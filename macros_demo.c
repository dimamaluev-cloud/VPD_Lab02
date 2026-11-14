/*
 * macros_demo.c
 * Лабораторная работа №2, раздел 14:
 * const и #define, параметризованные макросы и их ловушки,
 * stringify (#), token pasting (##), условная компиляция (DEBUG).
 */

#include <stddef.h>   /* size_t */
#include <stdio.h>

/* =====================================================================
 * 14. const и #define - разные инструменты
 * ===================================================================== */

/* Макрос: препроцессор заменяет ТЕКСТ BUFFER_SIZE на 304 ещё до компиляции.
 * Это не переменная: у неё нет типа, адреса и области видимости. */
#define BUFFER_SIZE 304

/* const-объект: настоящая переменная языка с типом (size_t), адресом
 * и областью видимости, значение которой нельзя менять.
 * Оптимизатор может полностью убрать её из программы, поэтому
 * фраза "const всегда занимает память" слишком упрощена. */
const size_t buffer_size = 304;

/* =====================================================================
 * 14.1. Параметризованные макросы и ловушки
 * ===================================================================== */

/* Плохой макрос: параметр x подставляется как ТЕКСТ без скобок.
 * BAD_SQUARE(2 + 3) превращается в  2 + 3 * 2 + 3  = 2 + 6 + 3 = 11,
 * потому что * имеет более высокий приоритет, чем +. */
#define BAD_SQUARE(x) x * x

/* Хороший макрос: скобки вокруг каждого параметра и вокруг всего выражения.
 * GOOD_SQUARE(2 + 3) превращается в  ((2 + 3) * (2 + 3))  = 25. */
#define GOOD_SQUARE(x) ((x) * (x))

/* =====================================================================
 * 14.2. Stringify, token pasting и условная компиляция
 * ===================================================================== */

/* Оператор # превращает аргумент макроса в строковый литерал (stringify).
 * Аргумент НЕ вычисляется, превращается в текст как есть. */
#define STRINGIFY(x) #x

/* Оператор ## склеивает два токена в один (token pasting):
 * CONCAT(value, 304) превращается в идентификатор value304. */
#define CONCAT(a, b) a##b

/* Условная компиляция: что именно останется в программе, решает ПРЕПРОЦЕССОР
 * до компиляции (в отличие от runtime-проверки if, которая выполняется
 * при работе программы).
 *   - сборка с -DDEBUG: DEBUG_LOG печатает сообщение в stderr;
 *   - обычная сборка: DEBUG_LOG превращается в пустое выражение ((void)0),
 *     то есть вызов исчезает из программы полностью. */
#ifdef DEBUG
#  define DEBUG_LOG(msg) fprintf(stderr, "DEBUG: %s\n", msg)
#else
#  define DEBUG_LOG(msg) ((void)0)
#endif

int main(void)
{
    /* ---------- const против #define ---------- */
    printf("=== const vs #define ===\n");
    printf("BUFFER_SIZE (macro)       = %d\n", BUFFER_SIZE);
    printf("buffer_size (const object) = %zu\n", buffer_size);

    /* У const-объекта есть адрес: его можно взять оператором &.
     * Для макроса это невозможно: &BUFFER_SIZE превратилось бы в &304 - ошибка. */
    printf("address of buffer_size     = %p\n", (const void *)&buffer_size);

    /* ---------- BAD_SQUARE и GOOD_SQUARE ---------- */
    printf("\n=== BAD_SQUARE vs GOOD_SQUARE ===\n");
    printf("BAD_SQUARE(2 + 3)  = %d  (expands to 2 + 3 * 2 + 3)\n", BAD_SQUARE(2 + 3));
    printf("GOOD_SQUARE(2 + 3) = %d  (expands to ((2 + 3) * (2 + 3)))\n", GOOD_SQUARE(2 + 3));

    /* ---------- Побочные эффекты в аргументе макроса ---------- */
    printf("\n=== side effects: GOOD_SQUARE(i++) (UNDEFINED BEHAVIOR) ===\n");

    /* GOOD_SQUARE(i++) раскрывается в ((i++) * (i++)): переменная i изменяется
     * ДВАЖДЫ без точки следования между изменениями -> undefined behavior.
     * Макрос НЕ функция: аргумент подставляется как текст и вычисляется
     * столько раз, сколько раз x встречается в теле макроса. Даже правильные
     * скобки этого не исправляют. Число ниже - НЕ правильный ответ. */
    int i = 3;
    int result = GOOD_SQUARE(i++);
    printf("i = 3; GOOD_SQUARE(i++) -> result = %d, i = %d  (do NOT rely on this)\n", result, i);

    /* ---------- STRINGIFY и CONCAT ---------- */
    printf("\n=== stringify and token pasting ===\n");

    /* Аргумент STRINGIFY не вычисляется: 2 + 3 превращается в текст "2 + 3". */
    printf("STRINGIFY(hello)  = %s\n", STRINGIFY(hello));
    printf("STRINGIFY(2 + 3)  = %s\n", STRINGIFY(2 + 3));

    /* CONCAT склеивает токены: объявляем переменную с именем value304. */
    int CONCAT(value, 304) = 7;
    printf("CONCAT(value, 304) -> variable value304 = %d\n", value304);

    /* ---------- Условная компиляция ---------- */
    printf("\n=== conditional compilation ===\n");
#ifdef DEBUG
    printf("compiled WITH -DDEBUG\n");
#else
    printf("compiled WITHOUT -DDEBUG\n");
#endif
    DEBUG_LOG("this message appears only in a DEBUG build");

    return 0;
}
