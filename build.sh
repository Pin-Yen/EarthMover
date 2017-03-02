g++ -c -std=c++11 main.cpp -o main.o
g++ -c -std=c++11 displayboard.cpp -o displayboard.o
g++ -std=c++11 displayboard.o main.o -o FiveInARow