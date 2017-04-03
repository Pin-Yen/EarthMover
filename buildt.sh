g++ -c -std=c++11 -pthread main.cpp -o main.o -O3 -DTIME
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -O3
g++ -c -std=c++11 gomoku/freestyle/virtualboard.cpp -o virtualboard.o -O3
g++ -c -std=c++11 gomoku/freestyle/point.cpp -o point.o -g -O3
g++ -c -std=c++11 gomoku/freestyle/evaluator.cpp -o evaluator.o -O3
g++ -c -std=c++11 gomoku/freestyle/typetree.cpp -o typetree.o -O3
g++ -c -std=c++11 gametree.cpp -o gametree.o -O3
g++ -c -std=c++11 node.cpp -o node.o -O3

g++ -std=c++11 -pthread displayboard.o virtualboard.o point.o evaluator.o typetree.o gametree.o node.o main.o -o FiveInARowT -DTIME -O3