g++ -c -std=c++11 -pthread main.cpp -o main.o -g -DDEBUG
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -g -DDEBUG
g++ -c -std=c++11 gomoku/freestyle/evaluatorfreestyle.cpp -o evaluatorfreestyle.o -g -DDEBUG
g++ -c -std=c++11 gametree.cpp -o gametree.o -g -DDEBUG
g++ -c -std=c++11 node.cpp -o node.o -g -DDEBUG
g++ -c -std=c++11 objectcounter.cpp -o objectcounter.o -g -DDEBUG

g++ -std=c++11 -pthread displayboard.o evaluatorfreestyle.o gametree.o\
                        node.o objectcounter.o main.o -o DebugEM -g -DDEBUG