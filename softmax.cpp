/* testing softmax*/

#include <iostream>
#include <iomanip>
#include <cmath>

int main()
{
    /* testing array */
    const int length = 5;
    const double array[length] = {1.2, 2.4, 3.2, 5, 6};

    std::cout << std::fixed << std::setprecision(4);

    std::cout << "{ ";
    for (int i = 0; i < length; ++i)
        std::cout << array[i] << " ";
    std::cout << "}\n=>\n";

    double sigma = 0;
    for (int i = 0; i < length; ++i)
        sigma += exp(array[i]);

    std::cout << "{ ";
    for (int i = 0; i < length; ++i)
        std::cout << (exp(array[i]) / sigma) << " ";
    std::cout << " }\n";
}