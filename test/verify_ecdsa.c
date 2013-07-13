#include <stdio.h>
#include <string.h>

#include "../ecc.h"
#include "testlib.h"

int test_vector(char *line) {
    EccPoint l_public;
    uint8_t l_private[NUM_ECC_DIGITS];

    uint8_t l_hash[NUM_ECC_DIGITS];
    uint8_t l_random[NUM_ECC_DIGITS];

    uint8_t l_r[NUM_ECC_DIGITS];
    uint8_t l_s[NUM_ECC_DIGITS];

    uint8_t l_r_out[NUM_ECC_DIGITS];
    uint8_t l_s_out[NUM_ECC_DIGITS];

    int r;

    r = get_vli(line, "sec=", l_private);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub.x=", l_public.x);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "pub.y=", l_public.y);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "random=", l_random);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "hash=", l_hash);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "l_r=", l_r);
    if (r < 0) return 0;
    line += r;

    r = get_vli(line, "l_s=", l_s);
    if (r < 0) return 0;
    line += r;

    if (!test_key(l_private, &l_public))
    {
        printf("Public key incorrect\n");
        printf("Private: ");
        vli_print(l_private, NUM_ECC_DIGITS);
        printf("\n");
        return 0;
    }

    if (!ecc_valid_public_key(&l_public))
    {
        printf("Not a valid public key!\n");
        return 0;
    }

    if (!ecdsa_sign(l_r_out, l_s_out, l_private, l_random, l_hash))
    {
        printf("ecdsa_sign() failed\n");
        return 0;
    }

    if (!ecdsa_verify(&l_public, l_hash, l_r_out, l_s_out))
    {
        printf("ecdsa_verify() failed\n");
        return 0;
    }

    if (memcmp(l_r, l_r_out, NUM_ECC_DIGITS) != 0 ||
        memcmp(l_s, l_s_out, NUM_ECC_DIGITS) != 0)
    {
        printf("Computed r and s do not match test vector\n");
        printf("%-15s", "r:");
        vli_print(l_r, NUM_ECC_DIGITS);
        printf("\n%-15s", "computed r:");
        vli_print(l_r_out, NUM_ECC_DIGITS);
        printf("\n%-15s", "s:");
        vli_print(l_s, NUM_ECC_DIGITS);
        printf("\n%-15s", "computed s:");
        vli_print(l_s_out, NUM_ECC_DIGITS);
        printf("\n");
        return 0;
    }

    return 1;
}

int random_mode(void) {
    EccPoint l_public;
    uint8_t l_private[NUM_ECC_DIGITS];

    uint8_t l_hash[NUM_ECC_DIGITS];
    uint8_t l_random[NUM_ECC_DIGITS];

    uint8_t r[NUM_ECC_DIGITS];
    uint8_t s[NUM_ECC_DIGITS];

    int i;

    printf("Testing 256 signatures\n");

    for(i=0; i<256; ++i)
    {
        printf(".");
        fflush(stdout);
        getRandomBytes((char *)l_private, NUM_ECC_DIGITS * sizeof(uint8_t));

        ecc_make_key(&l_public, l_private, l_private);

        getRandomBytes((char *)l_hash, NUM_ECC_DIGITS * sizeof(uint8_t));
        getRandomBytes((char *)l_random, NUM_ECC_DIGITS * sizeof(uint8_t));

        if (!ecdsa_sign(r, s, l_private, l_random, l_hash))
        {
            printf("ecdsa_sign() failed\n");
            continue;
        }

        if (!ecc_valid_public_key(&l_public))
        {
            printf("Not a valid public key!\n");
            continue;
        }

        if (!ecdsa_verify(&l_public, l_hash, r, s))
        {
            printf("ecdsa_verify() failed\n");
        }
    }
    printf("\n");

    return 1;
}
