#include <stdio.h>

int main(void)
{
    int number;
    unsigned int unsigned_number;
    double real;

    /* 1. Чтение int. %d, а переменную передаём через адрес (&number). */
    printf("Enter an integer: ");
    if (scanf("%d", &number) != 1) {
        /* stderr - отдельный поток для сообщений об ошибках */
        fprintf(stderr, "Invalid integer input\n");
        return 1;   /* ненулевой код возврата = программа завершилась с ошибкой */
    }

    /* 2. Чтение unsigned int. Спецификатор %u. */
    printf("Enter an unsigned integer: ");
    if (scanf("%u", &unsigned_number) != 1) {
        fprintf(stderr, "Invalid unsigned integer input\n");
        return 1;
    }

    /* 3. Чтение double. Для scanf нужен %lf (для printf хватало %f). */
    printf("Enter a real number: ");
    if (scanf("%lf", &real) != 1) {
        fprintf(stderr, "Invalid real number input\n");
        return 1;
    }

    printf("You entered: int = %d, unsigned = %u, double = %.3f\n",
           number, unsigned_number, real);

    return 0;
}