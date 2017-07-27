# About
EarthMover is a gomoku (five in a row) AI program. 

The strategy currently adapted by EarthMover is Monte-Carlo Tree Search (MCTS).

EarthMover's architecture is based on the client-server model. The client is a webpage displayed on a browser. Requests made by the client
are transmitted via HTTP to the server, which runs the MCTS process and then replys the client with its decisions.

# How to install and run
## Requirements
- A computer or virtual machine running POSIX compatible operating systems. (Since we're using POSIX sockets & pthreads)
- GNU make.
- A version of GNU g++ compiler that supports c++11.

## Install and run
1. Clone or download the repository.
2. cd into the project directory and run `make` on terminal
3. Run `./EM` on terminal. You should see the messsage `Listening on port 1202`. (Or alternatively on 1302 or 1602)
4. Open your favorite browser and navigate to localhost:1202 (or 1302, 1602. Depends on which port EM is listening on.)
5. Voila ! You can now challenge EarthMover.
