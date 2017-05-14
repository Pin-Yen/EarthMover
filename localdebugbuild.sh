g++ -c -std=c++11 -pthread main.cpp -o main.o -g -DDEBUG
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -g -DDEBUG
g++ -c -std=c++11 gomoku/freestyle/virtualboard.cpp -o virtualboard.o -g -DDEBUG
g++ -c -std=c++11 gomoku/freestyle/point.cpp -o point.o -g -DDEBUG
g++ -c -std=c++11 gomoku/freestyle/evaluator.cpp -o evaluator.o -g -DDEBUG
g++ -c -std=c++11 gomoku/freestyle/typetree.cpp -o typetree.o -g -DDEBUG
g++ -c -std=c++11 gomoku/openingtree.cpp -o openingtree.o -g -DDEBUG
g++ -c -std=c++11 gametree.cpp -o gametree.o -g -DDEBUG
g++ -c -std=c++11 node.cpp -o node.o -g -DDEBUG
g++ -c -std=c++11 objectcounter.cpp -o objectcounter.o -g -DDEBUG
g++ -c -std=c++11 log.cpp -o log.o -g -DDEBUG

g++ -std=c++11 -pthread displayboard.o virtualboard.o point.o evaluator.o typetree.o\
                        openingtree.o gametree.o node.o objectcounter.o log.o main.o -o DebugEM -g -DDEBUG