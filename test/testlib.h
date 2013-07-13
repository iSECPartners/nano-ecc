#ifndef __TESTLIB_H__

void getRandomBytes(void *p_dest, unsigned p_size);
void vli_print(uint8_t *p_vli, unsigned int p_size);
int get_vli(char *buf, char *key, uint8_t vli[NUM_ECC_DIGITS]);
int test_key(uint8_t secret[NUM_ECC_DIGITS], EccPoint *pub);

#endif /* __TESTLIB_H__ */
