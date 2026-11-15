#include <iostream>

int main()
{
    int numbers[] = {10, 20, 30, 40, 50};

    std::cout << "Array elements:" << std::endl;

    for (int value : numbers)
    {
        std::cout << value << std::endl;
    }

    int sum = 0;

    for (int value : numbers)
    {
        sum += value;
    }

    std::cout << "Sum = " << sum << std::endl;

    return 0;
}