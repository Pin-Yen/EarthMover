g++ -c -std=c++11 main.cpp -o main.o
g++ -c -std=c++11 chessboard.cpp -o chessboard.o
g++ -c -std=c++11 point.cpp -o point.o
g++ -std=c++11 chessboard.o main.o point.o -o FiveInARow