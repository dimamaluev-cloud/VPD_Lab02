#include <stddef.h>   /* size_t */
#include <stdio.h>

int main(void)
{
    /* ================= Часть A: неинициализированные массивы ================= */
    {   /* Блок: переменные ниже существуют только внутри этих скобок */

        /* Локальные массивы без инициализатора: компилятор не обязан
         * записывать в них какие-либо значения. */
        char chars[304];
        int integers[25];
        double reals[25];

        printf("=== Part A: UNINITIALIZED arrays (indeterminate values, UB) ===\n");

        /* Печатаем первые 10 элементов. char выводим как число (%d),
         * чтобы не печатать "мусорные" символы. */
        printf("chars[0..9]    :");
        for (size_t i = 0; i < 10; ++i) {
            printf(" %d", chars[i]);
        }
        printf("\n");

        printf("integers[0..9] :");
        for (size_t i = 0; i < 10; ++i) {
            printf(" %d", integers[i]);
        }
        printf("\n");

        printf("reals[0..9]    :");
        for (size_t i = 0; i < 10; ++i) {
            printf(" %g", reals[i]);   /* %g - краткая запись вещественного числа */
        }
        printf("\n\n");
    }   /* Здесь массивы части A перестают существовать */

    /* ================= Часть B: инициализированные массивы ================= */

    /* {0} обнуляет ВЕСЬ массив: первый элемент получает 0,
     * остальные элементы тоже заполняются нулями. */
    char chars[304] = {0};
    int integers[25] = {0};
    double reals[25] = {0};

    printf("=== Part B: INITIALIZED arrays ===\n");

    /* Число элементов = размер всего массива / размер одного элемента */
    size_t chars_count = sizeof(chars) / sizeof(chars[0]);
    size_t integers_count = sizeof(integers) / sizeof(integers[0]);
    size_t reals_count = sizeof(reals) / sizeof(reals[0]);

    /* Заполняем осмысленными значениями */
    for (size_t i = 0; i < chars_count; ++i) {
        chars[i] = (char)('A' + i % 26);     /* буквы A..Z по кругу */
    }
    for (size_t i = 0; i < integers_count; ++i) {
        integers[i] = (int)(i * i);          /* квадраты: 0, 1, 4, 9, ... */
    }
    for (size_t i = 0; i < reals_count; ++i) {
        reals[i] = (double)i / 2.0;          /* 0.0, 0.5, 1.0, 1.5, ... */
    }

    /* Числовые массивы: по одному элементу в строке */
    printf("\n-- integers (one per line) --\n");
    for (size_t i = 0; i < integers_count; ++i) {
        printf("integers[%zu] = %d\n", i, integers[i]);
    }

    printf("\n-- reals (one per line) --\n");
    for (size_t i = 0; i < reals_count; ++i) {
        printf("reals[%zu] = %.2f\n", i, reals[i]);
    }

    /* char-массив: таблица шириной 10 элементов.
     * После каждого десятого элемента переходим на новую строку. */
    printf("\n-- chars (10 per row) --\n");
    for (size_t i = 0; i < chars_count; ++i) {
        printf("%c ", chars[i]);
        if ((i + 1) % 10 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    /* Для каждого массива: размер всего массива, размер элемента, число элементов */
    printf("\n-- sizes --\n");
    printf("chars:    sizeof(array) = %zu, sizeof(element) = %zu, count = %zu\n",
           sizeof(chars), sizeof(chars[0]), chars_count);
    printf("integers: sizeof(array) = %zu, sizeof(element) = %zu, count = %zu\n",
           sizeof(integers), sizeof(integers[0]), integers_count);
    printf("reals:    sizeof(array) = %zu, sizeof(element) = %zu, count = %zu\n",
           sizeof(reals), sizeof(reals[0]), reals_count);

    return 0;
}