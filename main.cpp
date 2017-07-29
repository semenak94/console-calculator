#include "calculator.h"
#include <iostream>
#include <exception>

int main()
{
    auto tests = { "3+4*2/(1-5)^2^3",
                   "20    -30/  3 +4  *2   ^3",
                    "-1 + 5 - 3",
                    "-10 + (8 * 2.5) - (    3 / 1,5)",
                    "1+(2*   (2.5+2,5+  (3-   2)))-(3/1,5)",
                    "1.1 + 2.1 + abc" };

    try {
        for (std::string item : tests) {
            calculator(item);
            std::cout << "OK" << std::endl;
        }
    }
    catch (std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}
