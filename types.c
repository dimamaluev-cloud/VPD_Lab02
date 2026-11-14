#include <limits.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    printf("=== sizeof ===\n");
    printf("char               = %zu\n", sizeof(char));
    printf("signed char        = %zu\n", sizeof(signed char));
    printf("unsigned char      = %zu\n", sizeof(unsigned char));
    printf("short              = %zu\n", sizeof(short));
    printf("unsigned short     = %zu\n", sizeof(unsigned short));
    printf("int                = %zu\n", sizeof(int));
    printf("unsigned int       = %zu\n", sizeof(unsigned int));
    printf("long               = %zu\n", sizeof(long));
    printf("unsigned long      = %zu\n", sizeof(unsigned long));
    printf("long long          = %zu\n", sizeof(long long));
    printf("unsigned long long = %zu\n", sizeof(unsigned long long));
    printf("float              = %zu\n", sizeof(float));
    printf("double             = %zu\n", sizeof(double));
    printf("long double        = %zu\n", sizeof(long double));
    printf("void *             = %zu\n", sizeof(void *));
    printf("uint8_t            = %zu\n", sizeof(uint8_t));
    printf("uint16_t           = %zu\n", sizeof(uint16_t));
    printf("uint32_t           = %zu\n", sizeof(uint32_t));
    printf("uint64_t           = %zu\n", sizeof(uint64_t));

    printf("\n=== limits ===\n");
    printf("INT_MIN    = %d\n", INT_MIN);
    printf("INT_MAX    = %d\n", INT_MAX);
    printf("UINT_MAX   = %u\n", UINT_MAX);
    printf("UINT8_MAX  = %llu\n", (unsigned long long)UINT8_MAX);
    printf("UINT32_MAX = %llu\n", (unsigned long long)UINT32_MAX);

    printf("\n=== number 304 ===\n");
    int a = 304;
    int b = 0460;
    int c = 0x130;
    printf("dec=%d oct=%o hex=%X\n", a, a, a);
    printf("a == b: %d, a == c: %d\n", a == b, a == c);

    printf("\n=== powers of two ===\n");
    for (int i = 0; i <= 20; ++i) {
        printf("2^%-2d = %llu\n", i, 1ULL << i);
    }
    printf("2^32 = %llu\n", 1ULL << 32);
    printf("2^48 = %llu\n", 1ULL << 48);
    printf("2^63 = %llu\n", 1ULL << 63);
    printf("2^64 = it does not fit unsigned long long\n");

    return 0;
}