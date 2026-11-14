#include <stddef.h>   /* определение NULL (нужно для gettimeofday на Linux/macOS) */

#include "timer.h"    /* свой заголовок: проверка совпадения прототипа */

/* ---------- Условная компиляция ----------
 * #ifdef _WIN32 ... #else ... #endif выполняется ПРЕПРОЦЕССОРОМ до компиляции.
 * Макрос _WIN32 автоматически определяют компиляторы под Windows (включая MinGW),
 * поэтому на Windows в программу попадёт первая ветка, а вторая будет
 * полностью выброшена (и наоборот на Linux/macOS). Это не runtime-проверка
 * (не if во время работы), выбор делается один раз при сборке. */
#ifdef _WIN32
#include <windows.h>   /* типы и функции Windows API: LARGE_INTEGER, QueryPerformance... */

double highres_seconds(void)
{
    /* LARGE_INTEGER - 64-битное целое Windows (union); значение лежит в поле QuadPart. */
    LARGE_INTEGER counter;     /* текущее значение аппаратного счётчика (в "тиках") */
    LARGE_INTEGER frequency;   /* сколько тиков счётчика проходит за одну секунду */

    /* Частота счётчика не меняется во время работы системы, но для простоты
     * спрашиваем её при каждом вызове. */
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);

    /* Секунды = тики / (тики в секунду). Приведение к double обязательно:
     * иначе целочисленное деление отбросило бы дробную часть. */
    return (double)counter.QuadPart / (double)frequency.QuadPart;
}
#else
#include <sys/time.h>   /* struct timeval и функция gettimeofday (Linux/macOS) */

double highres_seconds(void)
{
    struct timeval tv;   /* tv_sec - целые секунды, tv_usec - микросекунды */

    /* Второй аргумент (часовой пояс) устарел, передаём NULL. */
    gettimeofday(&tv, NULL);

    /* Секунды + микросекунды / 1 000 000 = время как дробное число секунд. */
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}
#endif
