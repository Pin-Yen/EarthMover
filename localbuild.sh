g++ -c -std=c++11 -pthread main.cpp -o main.o -Wall -O3
g++ -c -std=c++11 gomoku/displayboard.cpp -o displayboard.o -Wall -O3
g++ -c -std=c++11 gomoku/freestyle/evaluatorfreestyle.cpp -o -Wall evaluatorfreestyle.o -O3
g++ -c -std=c++11 gametree.cpp -o gametree.o -Wall -O3
g++ -c -std=c++11 node.cpp -o node.o -Wall -O3

g++ -std=c++11 -pthread displayboard.o evaluatorfreestyle.o gametree.o node.o main.o -o LocalEM -Wall -O3