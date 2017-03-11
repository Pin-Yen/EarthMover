g++ -c -std=c++11 main.cpp -o main.o
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o
g++ -c -std=c++11 gomoku/freestyle/virtualboard.cpp -o virtualboard.o
g++ -c -std=c++11 gomoku/freestyle/point.cpp -o point.o
g++ -c -std=c++11 gomoku/freestyle/evaluator.cpp -o evaluator.o
g++ -c -std=c++11 gomoku/freestyle/typetree.cpp -o typetree.o
g++ -std=c++11 displayboard.o virtualboard.o point.o evaluator.o typetree.o main.o -o FiveInARow