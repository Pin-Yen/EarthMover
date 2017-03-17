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

	memset(array, sizeof(array), 12);

	diration = (std::clock() - start);
	
	std::cout << diration / CLOCKS_PER_SEC << std::endl;

	std::cout << array[1848] << "\n" << array[378475] << "\n";

/*
	start = std::clock();

	for (int i = 0; i < size; ++i)
		array[i] = 1;


	diration = (std::clock() - start);

	std::cout << diration / CLOCKS_PER_SEC << std::endl;

	

	start = std::clock();

	std::fill(array, array + size, 3);

	diration = (std::clock() - start);
	
	std::cout << diration / CLOCKS_PER_SEC << std::endl;

*/
}