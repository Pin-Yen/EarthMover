#include <iostream>
#include <cstdio>
#include <cstring>
#include <ctime>

int main()
{

	const int size = 1000000;
	int array[size];

	std::clock_t start;
	double diration;

	start = std::clock();

	for (int i = 0; i < size; ++i)
		array[i] = 1;


	diration = (std::clock() - start);

	std::cout << diration / CLOCKS_PER_SEC << std::endl;

	start = std::clock();

	memset(array, 2, sizeof(array));

	diration = (std::clock() - start);
	
	std::cout << diration / CLOCKS_PER_SEC << std::endl;

	start = std::clock();

	std::fill(array, array + size, 3);

	diration = (std::clock() - start);
	
	std::cout << diration / CLOCKS_PER_SEC << std::endl;
}