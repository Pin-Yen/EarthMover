gcc -c -std=c++98 main.cpp -o main.o
gcc -c -std=c++98 chessboard.cpp -o chessboard.o
gcc -std=c++98 chessboard.o main.o -o FiveInARow