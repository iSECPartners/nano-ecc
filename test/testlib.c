#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../ecc.h"

int test_vector(char *line);
int random_mode(void);

int randfd;

void getRandomBytes(void *p_dest, unsigned p_size)
{
    if(read(randfd, p_dest, p_size) != (int)p_size)
    {
        printf("Failed to get random bytes.\n");
    }
}

void vli_print(uint8_t *p_vli, unsigned int p_size)
{
    while(p_size)
    {
        printf("%02X ", p_vli[p_size - 1]);
        --p_size;
    }
}

int get_vli(char *buf, char *key, uint8_t vli[NUM_ECC_DIGITS]) {
    int i;
    uint8_t bytes[NUM_ECC_DIGITS];
    char *found = strstr(buf, key);
    if (!found)
    {
        printf("Can't find %s\n", key);
        return -1;
    }
    found += strlen(key);
    if (strlen(found) < 2 * NUM_ECC_DIGITS)
    {
        printf("Error: string too short");
        return -1;
    }

    for (i = 0; i < NUM_ECC_DIGITS; ++i)
    {
        unsigned chr;
        if (!isxdigit(found[2*i]) || !isxdigit(found[2*i+1]))
        {
            printf("Error: '%c%c' is not hex\n", found[2*i], found[2*i+1]);
            return -1;
        }
        sscanf(&found[2*i], "%02x", &chr);
        bytes[i] = chr;
    }

    ecc_bytes2native(vli, bytes);

    return found + 2*NUM_ECC_DIGITS - buf;
}

int test_key(uint8_t secret[NUM_ECC_DIGITS], EccPoint *pub) {
    EccPoint test_pub;
    ecc_make_key(&test_pub, secret, secret);
    return (memcmp(pub->x, test_pub.x, NUM_ECC_DIGITS) == 0) &&
           (memcmp(pub->y, test_pub.y, NUM_ECC_DIGITS) == 0);
}

int file_mode(char *filename) {
    int total = 0;
    char buf[4096];
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("Unable to open %s: %s\n", filename, strerror(errno));
        return 0;
    }

    char *curve = "curve="
#if ECC_CURVE == secp128r1
        "secp128r1";
#elif ECC_CURVE == secp192r1
        "secp192r1";
#elif ECC_CURVE == secp256r1
        "secp256r1";
#elif ECC_CURVE == secp384r1
        "secp384r1";
#else
#error Unknown curve
#endif

    while (fgets(buf, sizeof(buf), f))
    {
        if (strstr(buf, curve) != NULL)
        {
            break;
        }
    }
    if (feof(f))
    {
        printf("Could not find test vectors for %s\n", curve);
        return 0;
    }

    while (fgets(buf, sizeof(buf), f)) {
        int r;
        char *nl;

        if (strstr(buf, "curve=") != NULL)
        {
            break;
        }

        printf(".");
        fflush(stdout);

        nl = strchr(buf, '\n');
        if (nl == NULL)
        {
            printf("Error: line too long\n");
            return 0;
        }
        *nl = '\0';

        r = test_vector(buf);
        if (!r)
        { 
            break;
        }

        ++total;
    }

    printf("\nPassed %d tests\n", total);
    return 1;
}

void usage(char *name) {
    printf("Usage: %s < -r | -f <filename> >\n", name);
    printf("    -r  generate random test vectors\n");
    printf("    -f  load test vectors from file\n");
    exit(1);
}

int main(int argc, char **argv)
{
    int do_random = 0, do_file = 0;
    char *filename = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "hrf:")) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'r':
                do_random = 1;
                break;
            case 'f':
                do_file = 1;
                filename = strdup(optarg);
                break;
            default: /* '?' */
                usage(argv[0]);
        }
    }

    if (!do_file && !do_random)
    {
        usage(argv[0]);
    }

    if (do_file)
    {
        file_mode(filename);
    }

    if (do_random)
    {
        randfd = open("/dev/urandom", O_RDONLY);
        if(randfd == -1)
        {
            printf("No access to urandom\n");
            return 1;
        }

        random_mode();
    }

    return 0;
}
