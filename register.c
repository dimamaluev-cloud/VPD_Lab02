#include <stdint.h>   /* uint8_t */
#include <stdio.h>

/* Каждый бит регистра отвечает за своё состояние.
 * 1u << n - единица, сдвинутая на n бит влево, то есть число,
 * у которого включён только бит с номером n (нумерация с нуля справа):
 *   1u << 0 = 00000001, 1u << 1 = 00000010, 1u << 2 = 00000100 ... */
enum RegisterBit {
    REG_POWER = 1u << 0,   /* питание включено    */
    REG_READY = 1u << 1,   /* устройство готово   */
    REG_ERROR = 1u << 2,   /* ошибка              */
    REG_TX    = 1u << 3,   /* идёт передача       */
    REG_RX    = 1u << 4    /* идёт приём          */
};

/* Печать 8-битного значения в двоичном виде, старший бит слева. */
static void print_binary8(uint8_t value)
{
    for (int bit = 7; bit >= 0; --bit) {
        /* (value & (1u << bit)) оставляет только бит с номером bit:
         * если он равен 1 - результат не ноль, печатаем '1', иначе '0'. */
        putchar((value & (1u << bit)) ? '1' : '0');
    }
}

/* Печать регистра в десятичном, шестнадцатеричном и двоичном виде. */
static void print_reg(const char *label, uint8_t reg)
{
    printf("%-22s dec = %3u, hex = 0x%02X, bin = ", label, (unsigned)reg, (unsigned)reg);
    print_binary8(reg);
    printf("\n");
}

int main(void)
{
    uint8_t reg = 0;   /* все биты сброшены */

    printf("=== 8-bit device register ===\n");
    print_reg("start", reg);

    /* УСТАНОВИТЬ бит: reg |= MASK.
     * Операция ИЛИ (|): бит результата равен 1, если он равен 1 хотя бы в одном
     * из операндов. Биты, не входящие в маску, не меняются. */
    reg |= REG_POWER;
    print_reg("set POWER", reg);
    reg |= REG_READY;
    print_reg("set READY", reg);
    reg |= REG_TX;
    print_reg("set TX", reg);

    /* СБРОСИТЬ бит: reg &= ~MASK.
     * ~MASK инвертирует маску (бит маски становится 0, все остальные 1),
     * а операция И (&) обнуляет только этот бит. Результат ~ имеет тип int,
     * поэтому для uint8_t делаем явное приведение. */
    reg &= (uint8_t)~REG_READY;
    print_reg("clear READY", reg);

    /* ПЕРЕКЛЮЧИТЬ бит: reg ^= MASK.
     * Исключающее ИЛИ (^) меняет бит на противоположный:
     * 0 -> 1, 1 -> 0. Повторное применение возвращает исходное значение. */
    reg ^= REG_ERROR;
    print_reg("toggle ERROR (1st)", reg);
    reg ^= REG_ERROR;
    print_reg("toggle ERROR (2nd)", reg);

    /* ПРОВЕРИТЬ бит: (reg & MASK) != 0.
     * Результат равен 1 (истина), если бит установлен, и 0, если сброшен. */
    printf("\n=== test bits ===\n");
    printf("POWER set? %d\n", (reg & REG_POWER) != 0);
    printf("READY set? %d\n", (reg & REG_READY) != 0);
    printf("ERROR set? %d\n", (reg & REG_ERROR) != 0);
    printf("TX    set? %d\n", (reg & REG_TX) != 0);
    printf("RX    set? %d\n", (reg & REG_RX) != 0);

    /* СФОРМИРОВАТЬ маску как 1u << n: для бита с номером 4 это маска REG_RX. */
    printf("\n=== mask as 1u << n ===\n");
    unsigned int n = 4;
    unsigned int mask = 1u << n;
    printf("n = %u, mask = 1u << n = %u (hex 0x%02X), equals REG_RX: %d\n",
           n, mask, mask, mask == REG_RX);
    reg |= (uint8_t)mask;   /* устанавливаем бит RX через вычисленную маску */
    print_reg("set bit n=4 (RX)", reg);

    return 0;
}