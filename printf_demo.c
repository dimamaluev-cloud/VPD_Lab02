#include <stddef.h>   /* size_t */
#include <stdio.h>

int main(void)
{
    /* ================= 7.1. Спецификаторы printf() ================= */
    printf("=== 7.1 printf specifiers ===\n");

    int number = 304;
    unsigned int unsigned_number = 304u;
    char letter = 'M';
    const char *text = "MAI304";
    double real = 3.14159;
    size_t size = sizeof(number);

    /* Одно целое число в трёх системах счисления:
     * %d - десятичное со знаком, %o - восьмеричное, %x / %X - шестнадцатеричное
     * (строчные / заглавные буквы a-f). */
    printf("decimal = %d, octal = %o, hex = %x, HEX = %X\n",
           number, number, number, number);

    printf("%%u  unsigned int   : %u\n", unsigned_number);   /* %% печатает знак % */
    printf("%%c  character      : %c\n", letter);
    printf("%%s  C string       : %s\n", text);
    printf("%%f  floating point : %f\n", real);
    printf("%%zu size_t         : %zu\n", size);

    /* %p печатает адрес. Аргумент для %p должен иметь тип void *,
     * поэтому делаем явное приведение. Адрес меняется от запуска к запуску. */
    printf("%%p  pointer        : %p\n", (void *)&number);

    /* ================= 7.2. Управляющие последовательности ================= */
    printf("\n=== 7.2 escape sequences ===\n");

    /* \t - горизонтальная табуляция: курсор переходит к следующей позиции табуляции */
    printf("AAA\tBBB\n");

    /* \r - возврат каретки в начало ТЕКУЩЕЙ строки: SECOND перепишет first.
     * В терминале видно "SECOND", а в файле при перенаправлении сохранится
     * и сам символ \r (управляющий символ). */
    printf("first\rSECOND\n");

    /* \0 - нулевой символ. Строка в C заканчивается на первом \0,
     * поэтому %s / printf остановится перед ним, и "two" не будет выведено.
     * Здесь формат "one\0two\n" - сама строка формата обрывается на \0,
     * поэтому не напечатаются ни "two", ни даже перевод строки. */
    printf("one\0two\n");
    printf("\n");   /* добавляем перевод строки вручную, чтобы вывод не склеился */

    /* \\ - обратный слеш, \" - двойная кавычка, \' - одинарная кавычка */
    printf("quote: \"C\" and slash: \\\n");
    printf("apostrophe: \'x\'\n");

    /* \v - вертикальная табуляция (эффект зависит от терминала) */
    printf("line1\vline2\n");

    /* \a - bell (звуковой сигнал); эффект зависит от терминала.
     * Здесь не вызываем, чтобы не было звука при запуске. */

    return 0;
}