/*
 * same_text.c
 * Лабораторная работа №2, раздел 6:
 * одинаковые символы (коды ASCII) в массивах разных целочисленных типов.
 *
 * Идея: 'M', 'A', 'I', '3', '0', '4' - это просто маленькие числа (коды символов).
 * Их можно хранить в char, short, int или unsigned long long.
 * Выведенный текст будет одинаковым, а размер массивов - разным.
 */

#include <stddef.h>   /* size_t */
#include <stdio.h>

int main(void)
{
    /* Четыре массива с ОДНИМИ И ТЕМИ ЖЕ кодами символов, но разным типом элемента.
     * Размер каждого массива компилятор считает сам по числу элементов в { }. */
    char a[] = {'M', 'A', 'I', '3', '0', '4'};
    short b[] = {'M', 'A', 'I', '3', '0', '4'};
    int c[] = {'M', 'A', 'I', '3', '0', '4'};
    unsigned long long d[] = {'M', 'A', 'I', '3', '0', '4'};

    /* ================= Часть 1: вывод поэлементно через %c ================= */
    printf("=== Part 1: print element by element with %%c ===\n");

    /* sizeof(массив) / sizeof(элемент) - число элементов массива.
     * %c ожидает значение типа char, поэтому для остальных типов делаем
     * явное приведение (char): значение (код символа) от этого не меняется. */
    printf("char:               ");
    for (size_t i = 0; i < sizeof(a) / sizeof(a[0]); ++i) {
        printf("%c", a[i]);
    }
    printf("\n");

    printf("short:              ");
    for (size_t i = 0; i < sizeof(b) / sizeof(b[0]); ++i) {
        printf("%c", (char)b[i]);
    }
    printf("\n");

    printf("int:                ");
    for (size_t i = 0; i < sizeof(c) / sizeof(c[0]); ++i) {
        printf("%c", (char)c[i]);
    }
    printf("\n");

    printf("unsigned long long: ");
    for (size_t i = 0; i < sizeof(d) / sizeof(d[0]); ++i) {
        printf("%c", (char)d[i]);
    }
    printf("\n");

    /* ================= Сравнение sizeof ================= */
    printf("\n=== sizeof comparison ===\n");

    /* Элементов везде 6, но размер каждого элемента разный,
     * поэтому и весь массив занимает разное количество байт. */
    printf("char a[6]               : sizeof(array) = %2zu, sizeof(element) = %zu\n",
           sizeof(a), sizeof(a[0]));
    printf("short b[6]              : sizeof(array) = %2zu, sizeof(element) = %zu\n",
           sizeof(b), sizeof(b[0]));
    printf("int c[6]                : sizeof(array) = %2zu, sizeof(element) = %zu\n",
           sizeof(c), sizeof(c[0]));
    printf("unsigned long long d[6] : sizeof(array) = %2zu, sizeof(element) = %zu\n",
           sizeof(d), sizeof(d[0]));

    /* ================= Часть 2: попытка вывода через %s ================= */
    printf("\n=== Part 2: try %%s after adding '\\0' ===\n");

    /* Добавляем ещё один элемент - нулевой символ '\0' (код 0).
     * Строка в C - это последовательность char, которая ЗАКАНЧИВАЕТСЯ байтом 0. */
    char a0[] = {'M', 'A', 'I', '3', '0', '4', '\0'};
    short b0[] = {'M', 'A', 'I', '3', '0', '4', '\0'};
    int c0[] = {'M', 'A', 'I', '3', '0', '4', '\0'};
    unsigned long long d0[] = {'M', 'A', 'I', '3', '0', '4', '\0'};

    /* char-массив - настоящая C-строка: %s читает байты до первого нуля. */
    printf("char:               \"%s\"\n", a0);

    /* Для остальных массивов приводим указатель к (char *): так %s читает
     * ОТДЕЛЬНЫЕ БАЙТЫ массива, а не элементы. Это допустимо: через указатель
     * на char можно читать байты любого объекта.
     *
     * На обычном (little-endian) компьютере элемент 'M' в short/int/ull хранится как
     * байт 0x4D, за которым идут нулевые байты. %s доходит до первого
     * нулевого байта и останавливается, поэтому печатается только "M". */
    printf("short:              \"%s\"\n", (char *)b0);
    printf("int:                \"%s\"\n", (char *)c0);
    printf("unsigned long long: \"%s\"\n", (char *)d0);

    /* Посмотрим на байты в памяти: первые 8 байт массива int.
     * unsigned char * - безопасный способ прочитать "сырые" байты объекта. */
    printf("\nFirst 8 bytes of int array c0 (hex):");
    const unsigned char *bytes = (const unsigned char *)c0;
    for (size_t i = 0; i < 8; ++i) {
        printf(" %02X", bytes[i]);
    }


printf("\n");

    /* Ожидаемо: 4D 00 00 00 41 00 00 00
     *   4D = 'M', 41 = 'A'; между ними нулевые байты (старшие байты int).
     * Вывод: массив int/short/unsigned long long не становится C-строкой
     * только потому, что его элементы равны кодам символов. */

    return 0;
}