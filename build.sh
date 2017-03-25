g++ -c -std=c++11 main.cpp -o main.o -g
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -g
g++ -c -std=c++11 gomoku/freestyle/virtualboard.cpp -o virtualboard.o -g
g++ -c -std=c++11 gomoku/freestyle/point.cpp -o point.o -g
g++ -c -std=c++11 gomoku/freestyle/evaluator.cpp -o evaluator.o -g
g++ -c -std=c++11 gomoku/freestyle/typetree.cpp -o typetree.o -g
g++ -c -std=c++11 gametree.cpp -o gametree.o -g
g++ -c -std=c++11 node.cpp -o node.o -g
g++ -c -std=c++11 objectcounter.cpp -o objectcounter.o -g
g++ -c -std=c++11 log.cpp -o log.o -g

g++ -std=c++11 displayboard.o virtualboard.o point.o evaluator.o typetree.o gametree.o node.o objectcounter.o log.o main.o -o FiveInARow -g