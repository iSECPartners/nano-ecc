#include <stdio.h>
#include <string.h>

#include "../ecc.h"
#include "testlib.h"

int test_vector(char *line) {
    EccPoint l_Q1, l_Q2; /* public keys */
    uint8_t l_secret1[NUM_ECC_DIGITS];
    uint8_t l_secret2[NUM_ECC_DIGITS];
    uint8_t l_shared1[NUM_ECC_DIGITS];
    uint8_t l_shared2[NUM_ECC_DIGITS];
    int r;

    r = get_vli(line, "sec1=", l_secret1);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub1.x=", l_Q1.x);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub1.y=", l_Q1.y);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "sec2=", l_secret2);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub2.x=", l_Q2.x);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub2.y=", l_Q2.y);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "shared=", l_shared1);
    if (r < 0) return 0;
    line += r;

    if (!test_key(l_secret1, &l_Q1))
    {
        printf("Public key 1 incorrect\n");
        printf("Private: ");
        vli_print(l_secret1, NUM_ECC_DIGITS);
        printf("\n");
        return 0;
    }
    if (!test_key(l_secret2, &l_Q2))
    {
        printf("Public key 2 incorrect\n");
        printf("Private: ");
        vli_print(l_secret2, NUM_ECC_DIGITS);
        printf("\n");
        return 0;
    }

    if (!ecdh_shared_secret(l_shared2, &l_Q1, l_secret2, NULL))
    {
        printf("Cannot compute shared secret\n");
        return 0;
    }
    if (memcmp(l_shared1, l_shared2, NUM_ECC_DIGITS) != 0) {
        printf("Shared secret incorrect\n");
        return 0;
    }
    if (!ecdh_shared_secret(l_shared2, &l_Q2, l_secret1, NULL))
    {
        printf("Cannot compute shared secret\n");
        return 0;
    }
    if (memcmp(l_shared1, l_shared2, NUM_ECC_DIGITS) != 0) {
        printf("Shared secret incorrect\n");
        return 0;
    }

    return 1;
}

int random_mode(void) {
    EccPoint l_Q1, l_Q2; /* public keys */
    uint8_t l_secret1[NUM_ECC_DIGITS];
    uint8_t l_secret2[NUM_ECC_DIGITS];
    int i;

    uint8_t l_shared1[NUM_ECC_DIGITS];
    uint8_t l_shared2[NUM_ECC_DIGITS];

    uint8_t l_random1[NUM_ECC_DIGITS];
    uint8_t l_random2[NUM_ECC_DIGITS];

    printf("Testing 256 random private key pairs\n");

    for(i=0; i<256; ++i)
    {
        printf(".");
        fflush(stdout);
        getRandomBytes((char *)l_secret1, NUM_ECC_DIGITS * sizeof(uint8_t));
        getRandomBytes((char *)l_secret2, NUM_ECC_DIGITS * sizeof(uint8_t));

        getRandomBytes((char *)l_random1, NUM_ECC_DIGITS * sizeof(uint8_t));
        getRandomBytes((char *)l_random2, NUM_ECC_DIGITS * sizeof(uint8_t));

        ecc_make_key(&l_Q1, l_secret1, l_secret1);
        ecc_make_key(&l_Q2, l_secret2, l_secret2);

        if (!ecdh_shared_secret(l_shared1, &l_Q1, l_secret2, l_random1))
        {
            printf("shared_secret() failed (1)\n");
            return 0;
        }

        if (!ecdh_shared_secret(l_shared2, &l_Q2, l_secret1, l_random2))
        {
            printf("shared_secret() failed (2)\n");
            return 0;
        }

        if (memcmp(l_shared1, l_shared2, sizeof(l_shared1)) != 0)
        {
            printf("Shared secrets are not identical!\n");
            printf("Shared secret 1 = ");
            vli_print(l_shared1, NUM_ECC_DIGITS);
            printf("\n");
            printf("Shared secret 2 = ");
            vli_print(l_shared2, NUM_ECC_DIGITS);
            printf("\n");
            printf("Private key 1 = ");
            vli_print(l_secret1, NUM_ECC_DIGITS);
            printf("\n");
            printf("Private key 2 = ");
            vli_print(l_secret2, NUM_ECC_DIGITS);
            printf("\n");
        }
    }
    printf("\n");

    return 1;
}
