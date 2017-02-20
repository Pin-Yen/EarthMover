g++ -c -std=c++98 main.cpp -o main.o
g++ -c -std=c++98 chessboard.cpp -o chessboard.o
g++ -std=c++98 chessboard.o main.o -o FiveInARow