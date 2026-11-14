/* ---------- Подключение заголовков ----------
 * <...> - системные заголовки стандартной библиотеки, "..." - заголовки проекта. */
#include <stdint.h>   /* uint8_t, uint64_t - типы фиксированной ширины */
#include <stdio.h>    /* printf, putchar, fflush */
#include <time.h>     /* time, difftime, clock, CLOCKS_PER_SEC, time_t, clock_t */

#include "device.h"   /* global_runs, device_tick, device_internal_calls */
#include "timer.h"    /* highres_seconds */

/* =====================================================================
 *                        НАСТРОЙКА НАГРУЗКИ
 * ===================================================================== */

/* Сколько значений обрабатывает КАЖДАЯ стадия.
 * По заданию не меньше 100 000. Число должно делиться на 100, иначе шаг
 * обновления progress bar (1 %) получится дробным.
 * Суффикс UL означает unsigned long: так число точно не переполнится. */
#define VALUES_PER_STAGE 200000UL

/* Сколько "раундов перемешивания" выполняется на ОДНО значение.
 * Именно это число определяет, сколько секунд работает программа.
 * ПОДБЕРИТЕ его так, чтобы полный запуск при -O0 занимал ориентировочно
 * 8-15 секунд, и запишите выбранное значение в отчёт.
 * #ifndef позволяет задать значение при сборке флагом -D, например:
 *   gcc -DROUNDS_PER_VALUE=5000UL ...
 * Если макрос не задан, используется значение по умолчанию ниже. */
#ifndef ROUNDS_PER_VALUE
#define ROUNDS_PER_VALUE 2500UL
#endif

/* Шаг обновления progress bar: один раз на каждый 1 % стадии.
 * Печатать на каждой итерации нельзя: вывод - это тоже работа программы,
 * он замедлил бы нагрузку и исказил измерение времени. */
#define PROGRESS_STEP (VALUES_PER_STAGE / 100UL)

/* =====================================================================
 *                   СТАДИИ (enum) И ФЛАГИ (битовая маска)
 * ===================================================================== */

/* Стадии работы. enum даёт читаемые имена вместо "магических" чисел 0, 1, 2, 3.
 * По умолчанию значения идут по порядку: STAGE_FOR = 0, STAGE_WHILE = 1, ... */
enum LabStage {
    STAGE_FOR,        /* обработка циклом for */
    STAGE_WHILE,      /* обработка циклом while */
    STAGE_DO_WHILE,   /* обработка циклом do...while */
    STAGE_DONE        /* признак "всё завершено" */
};

/* Флаги завершённых частей. Каждый флаг - это ОДИН бит (степень двойки):
 * 1u << n = число, у которого включён только бит с номером n.
 * Благодаря этому флаги можно хранить вместе в одном числе и комбинировать
 * операцией | (ИЛИ).
 *   FLAG_TYPES  = 000001, FLAG_ARRAYS = 000010, FLAG_BITS = 000100,
 *   FLAG_FOR    = 001000, FLAG_WHILE  = 010000, FLAG_DO   = 100000 (двоично). */
enum LabFlag {
    FLAG_TYPES  = 1u << 0,   /* раздел про типы выполнен */
    FLAG_ARRAYS = 1u << 1,   /* раздел про массивы выполнен */
    FLAG_BITS   = 1u << 2,   /* раздел про биты выполнен */
    FLAG_FOR    = 1u << 3,   /* стадия for завершена */
    FLAG_WHILE  = 1u << 4,   /* стадия while завершена */
    FLAG_DO     = 1u << 5    /* стадия do...while завершена */
};

/* Маска "все обязательные флаги": все шесть битов, объединённые операцией |.
 * Позже проверим, что в completed установлены ВСЕ эти биты. */
#define ALL_FLAGS \
    (FLAG_TYPES | FLAG_ARRAYS | FLAG_BITS | FLAG_FOR | FLAG_WHILE | FLAG_DO)

/* =====================================================================
 *                        ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
 * ===================================================================== */

/* print_progress: рисует progress bar в ОДНОЙ строке терминала.
 *   stage   - название стадии (печатается слева);
 *   percent - процент выполнения от 0 до 100.
 * static означает, что функция видна только в этом файле. */
static void print_progress(const char *stage, unsigned percent)
{
    const unsigned width = 30;                  /* ширина полоски в символах */
    unsigned filled = percent * width / 100;    /* сколько клеток закрашено '#' */

    /* \r (возврат каретки) ставит курсор в начало СТРОКИ без перевода на новую,
     * поэтому каждая новая печать затирает предыдущую - получается "анимация".
     * %-12s выводит название, выровняв по левому краю в поле шириной 12 символов. */
    printf("\r%-12s [", stage);

    for (unsigned i = 0; i < width; ++i) {
        /* Первые "filled" клеток закрашены, остальные пустые. */
        putchar(i < filled ? '#' : '-');
    }

    /* %3u - число шириной 3 символа (чтобы полоска не "прыгала"), %% - символ процента. */
    printf("] %3u%%", percent);

    /* В строке нет \n, поэтому вывод может застрять в буфере stdout и появиться
     * с задержкой. fflush(stdout) принудительно отправляет накопленное на экран. */
    fflush(stdout);
}

/* process_value: реальная вычислительная работа над одним значением.
 *   state - накопленное состояние (оно же итоговая контрольная сумма);
 *   value - номер текущего обрабатываемого значения.
 * Возвращает новое состояние. Все операции выполняются над uint64_t:
 * для беззнаковых типов переполнение определено стандартом как арифметика
 * по модулю 2^64 (то есть "оборачивание"), поэтому undefined behavior здесь нет. */
static uint64_t process_value(uint64_t state, uint64_t value)
{
    for (unsigned long r = 0; r < ROUNDS_PER_VALUE; ++r) {
        state ^= state << 13;   /* сдвиг влево на 13 бит и XOR с исходным значением */
        state ^= state >> 7;    /* сдвиг вправо на 7 бит и XOR */
        state ^= state << 17;   /* сдвиг влево на 17 бит и XOR */

        /* Сложение с произведением: умножение на большую "перемешивающую"
         * константу (число 0x9E3779B97F4A7C15 - дробная часть золотого сечения
         * в 64-битном виде, ULL = unsigned long long). */
        state += (value + r) * 0x9E3779B97F4A7C15ULL;
    }

    /* Результат используется дальше (идёт в checksum и печатается),
     * поэтому компилятор не может выбросить цикл как "пустой". */
    return state;
}

/* print_binary8: печатает младшие 8 бит числа в двоичном виде, старший бит слева. */
static void print_binary8(unsigned value)
{
    for (int bit = 7; bit >= 0; --bit) {
        /* (value & (1u << bit)) оставляет только бит с номером bit:
         * не ноль -> бит равен 1, ноль -> бит равен 0. */
        putchar((value & (1u << bit)) ? '1' : '0');
    }
}

/* =====================================================================
 *                             ГЛАВНАЯ ФУНКЦИЯ
 * ===================================================================== */

int main(void)
{
    /* Битовая маска завершённых частей. Изначально 0: ни один бит не установлен. */
    unsigned completed = 0;

    /* ---------- Часть 1: короткие проверки (выполняются ДО замера времени) ---------- */

    /* Типы: sizeof возвращает размер в байтах, тип результата size_t (формат %zu). */
    printf("sizeof(uint64_t) = %zu\n", sizeof(uint64_t));
    completed |= FLAG_TYPES;   /* |= устанавливает бит: включаем флаг FLAG_TYPES */

    /* Массивы: заполняем квадратами чисел и считаем сумму.
     * sizeof(numbers) / sizeof(numbers[0]) = число элементов массива. */
    int numbers[8];
    int sum = 0;
    for (size_t i = 0; i < sizeof(numbers) / sizeof(numbers[0]); ++i) {
        numbers[i] = (int)(i * i);   /* 0, 1, 4, 9, 16, 25, 36, 49 */
        sum += numbers[i];
    }
    printf("sum of squares 0..7 = %d\n", sum);   /* ожидаем 140 */
    completed |= FLAG_ARRAYS;

    /* Биты: условный регистр. Устанавливаем бит 1 и проверяем его. */
    uint8_t reg = 0;
    reg |= 1u << 1;                                  /* установить бит 1 */
    printf("register bit 1 set: %d\n", (reg & (1u << 1)) != 0);   /* ожидаем 1 */
    completed |= FLAG_BITS;

    printf("\nStarting workload: %lu values per stage, %lu rounds per value\n\n",
           VALUES_PER_STAGE, ROUNDS_PER_VALUE);

    /* ---------- Часть 2: запуск ВСЕХ трёх таймеров непосредственно перед нагрузкой ----------
     * Нагрузку выполняем ОДИН раз, а время измеряем сразу тремя способами. */

    time_t wall_start = time(NULL);        /* 1) time(): настенное время в целых секундах */
    clock_t cpu_start = clock();           /* 2) clock(): процессорное время программы (в тиках) */
    double hi_start = highres_seconds();   /* 3) высокоточный таймер (дробные секунды) */

    /* Начальное состояние вычислений (произвольное ненулевое число).
     * Нулевое состояние было бы плохим: сдвиги и XOR нуля дают ноль. */
    uint64_t checksum = 88172645463325252ULL;

    enum LabStage stage = STAGE_FOR;   /* начинаем с первой стадии */
    unsigned long i;                   /* счётчик значений; общий для while и do...while */

    /* ---------- Диспетчер стадий ----------
     * Внешний while крутится, пока не наступит STAGE_DONE.
     * switch выбирает действие по текущему значению enum.
     * Каждая стадия завершается присваиванием следующего значения stage,
     * поэтому цикл проходит стадии строго по порядку: FOR -> WHILE -> DO_WHILE -> DONE. */
    while (stage != STAGE_DONE) {
        switch (stage) {
        case STAGE_FOR:
            /* Цикл for: инициализация, условие и шаг собраны в одной строке.
             * Удобен, когда число повторений известно заранее. */
            for (i = 0; i < VALUES_PER_STAGE; ++i) {
                checksum = process_value(checksum, i);   /* полезная работа */
                device_tick();                           /* учёт работы в модуле device */

                /* i % PROGRESS_STEP == 0 истинно раз в PROGRESS_STEP итераций,
                 * то есть один раз на каждый 1 % (остаток от деления равен нулю). */
                if (i % PROGRESS_STEP == 0) {
                    print_progress("for", (unsigned)(i / PROGRESS_STEP));
                }
            }
            print_progress("for", 100);     /* итоговая полоска: ровно 100 % */
            printf("\n");                   /* перевод строки после завершения стадии */
            completed |= FLAG_FOR;          /* отметить стадию завершённой */
            stage = STAGE_WHILE;            /* переход к следующей стадии */
            break;                          /* break выходит из switch (не из while!) */

        case STAGE_WHILE:
            /* Цикл while: условие проверяется ДО тела.
             * Если условие сразу ложно, тело не выполнится ни разу.
             * Счётчик нужно объявить и изменять вручную. */
            i = 0;
            while (i < VALUES_PER_STAGE) {
                checksum = process_value(checksum, i);
                device_tick();
                if (i % PROGRESS_STEP == 0) {
                    print_progress("while", (unsigned)(i / PROGRESS_STEP));
                }
                ++i;   /* без этой строки цикл был бы бесконечным */
            }
            print_progress("while", 100);
            printf("\n");
            completed |= FLAG_WHILE;
            stage = STAGE_DO_WHILE;
            break;

        case STAGE_DO_WHILE:
            /* Цикл do...while: условие проверяется ПОСЛЕ тела,
             * поэтому тело гарантированно выполнится минимум один раз.
             * После while(...) обязательна точка с запятой. */
            i = 0;
            do {
                checksum = process_value(checksum, i);
                device_tick();
                if (i % PROGRESS_STEP == 0) {
                    print_progress("do...while", (unsigned)(i / PROGRESS_STEP));
                }
                ++i;
            } while (i < VALUES_PER_STAGE);
            print_progress("do...while", 100);
            printf("\n");
            completed |= FLAG_DO;
            stage = STAGE_DONE;
            break;

        case STAGE_DONE:
            /* Сюда программа не попадёт (внешний while уже завершится),
             * но ветка нужна: без неё gcc с -Wall предупредит, что не все
             * значения enum обработаны в switch. */
            break;
        }
    }

    /* ---------- Остановка таймеров сразу после нагрузки ---------- */
    time_t wall_end = time(NULL);
    clock_t cpu_end = clock();
    double hi_end = highres_seconds();

    /* ---------- Часть 3: результаты ---------- */

    /* Контрольная сумма: в десятичном и шестнадцатеричном виде.
     * Приведение к unsigned long long нужно, чтобы тип точно совпал с %llu / %llX.
     * %016llX - 16 hex-цифр с ведущими нулями (64 бита = 16 hex-цифр). */
    printf("\nChecksum: %llu (0x%016llX)\n",
           (unsigned long long)checksum, (unsigned long long)checksum);

    printf("\n=== Time measurement ===\n");

    /* 1) difftime возвращает разницу двух time_t в секундах как double.
     * Точность - целые секунды, поэтому способ грубый. */
    printf("time/difftime       : %.0f s\n", difftime(wall_end, wall_start));

    /* 2) clock() считает в "тиках". CLOCKS_PER_SEC - сколько тиков в одной секунде
     * (коэффициент перевода, а НЕ частота процессора в герцах).
     * Приведение к double обязательно, иначе деление целых отбросит дробную часть.
     * clock() измеряет процессорное время программы, а не реальное. */
    printf("clock/CLOCKS_PER_SEC: %.6f s  (CLOCKS_PER_SEC = %ld)\n",
           (double)(cpu_end - cpu_start) / CLOCKS_PER_SEC, (long)CLOCKS_PER_SEC);

    /* 3) Высокоточный таймер: разность двух замеров в дробных секундах. */
    printf("high-resolution time: %.6f s\n", hi_end - hi_start);

    /* ---------- Данные модуля device ---------- */
    printf("\n=== device module ===\n");
    /* global_runs определена в device.c, а здесь доступна через extern из device.h. */
    printf("global_runs (extern)        = %lu\n", global_runs);
    /* internal напрямую недоступна (static в device.c), берём через функцию. */
    printf("device_internal_calls()     = %lu\n", device_internal_calls());
    /* Ожидаем 3 * VALUES_PER_STAGE = 600000 в обоих случаях. */

    /* ---------- Итоговая маска флагов ---------- */
    printf("\n=== completed flags ===\n");
    printf("completed = 0x%02X, binary = ", completed);   /* %02X - hex минимум из 2 цифр */
    print_binary8(completed);
    printf("\n");

    /* (completed & ALL_FLAGS) оставляет только нужные биты;
     * если результат равен ALL_FLAGS, то установлены ВСЕ обязательные флаги. */
    printf("all required flags set: %s\n",
           (completed & ALL_FLAGS) == ALL_FLAGS ? "yes" : "NO");

    return 0;   /* 0 - программа завершилась успешно */
}
