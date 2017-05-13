g++ -c -std=c++11 networkmain.cpp -o networkmain.o -O3
g++ -c -std=c++11 httpserver.cpp -o httpserver.o -O3
g++ -c -std=c++11 -pthread ai.cpp -o ai.o -O3
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -O3
g++ -c -std=c++11 gomoku/freestyle/virtualboard.cpp -o virtualboard.o -O3
g++ -c -std=c++11 gomoku/freestyle/point.cpp -o point.o -g -O3
g++ -c -std=c++11 gomoku/freestyle/evaluator.cpp -o evaluator.o -O3
g++ -c -std=c++11 gomoku/openingtree.cpp -o openingtree.o -O3
g++ -c -std=c++11 gomoku/freestyle/typetree.cpp -o typetree.o -O3
g++ -c -std=c++11 gametree.cpp -o gametree.o -O3
g++ -c -std=c++11 node.cpp -o node.o -O3

g++ -std=c++11 -pthread displayboard.o virtualboard.o point.o evaluator.o typetree.o\
                        openingtree.o gametree.o node.o httpserver.o ai.o networkmain.o -o EM -O3