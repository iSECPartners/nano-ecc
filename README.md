nano-ecc
========

A very small ECDH and ECDSA implementation for 8-bit microcontrollers.

Based on kmackay's micro-ecc, a small ECDH and ECDSA implementation for 32-bit microcontrollers. For more information see https://github.com/kmackay/micro-ecc

Features
--------

 * Resistant to known side-channel attacks.
 * Written in C, with optional inline assembly forthcoming
 * Small code size: ECDH in as little as 6KB, ECDH + ECDSA in as little as 7KB
 * No dynamic memory allocation.
 * Reasonably fast: on an ATmega328P at 16MHz (AVR, 2-cycle 8x8 bit multiply), 192-bit ECDH shared secret calculation takes about 4034ms
 * Support for 4 standard curves: secp128r1, secp192r1, secp256r1, and secp384r1
 * BSD 2-clause license.

Usage Notes
-----------

#### Integer Representation ####

To reduce code size, all large integers are represented using little-endian bytes - so the least significant bytes is first. For example, the standard representation of the prime modulus for the curve secp128r1 is `FFFFFFFD FFFFFFFF FFFFFFFF FFFFFFFF`; in nano-ecc, this would be represented as `uint8_t p[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff 0xff, 0xff, 0xff, 0xff 0xff, 0xfd, 0xff, 0xff, 0xff};`.

You can use the `ecc_bytes2native()` and `ecc_native2bytes()` functions to convert between the native byte representation and the standardized octet representation.

#### Generating Keys ####

You can use the `makekeys` program in the `apps` directory to generate keys (on Linux or OS X). You can run `make` in that directory to build for your native platform. To generate a single public/private key pair, run `makekeys`. It will print out the public and private keys in a representation suitable to be copied into your source code. You can generate multiple key pairs at once using `makekeys <n>` to generate n keys.

#### Using the Code ####

I recommend just copying (or symlink) ecc.h and ecc.c into your project. Then just `#include "ecc.h"` to use the nano-ecc functions.

See ecc.h for documentation for each function.

Speed and Size
--------------

Available optimizations are:
 * `ECC_ENABLE_DSA` - Set to 0 to avoid including any DSA-related code.
 * `ECC_SQUARE_FUNC` - Use a separate function for squaring.
 * `ECC_ASM` - Choose the type of inline assembly to use. Currently only `ecc_asm_none` is available. In the near future we plan to provide `ecc_asm_avr` and `ecc_asm_8051`.

Comprehensive code size and performance analysis are forthcoming.
