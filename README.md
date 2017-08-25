# About
EarthMover is a gomoku (five in a row) AI program. 

The strategy currently adapted by EarthMover is Monte-Carlo Tree Search (MCTS). We are also expirementing with different methods. 

EarthMover's architecture is based on the client-server model. The client is a webpage displayed on a browser. Requests made by the client (browser) are transmitted via HTTP to the server, which runs the MCTS process and then replys the client with its decisions.

# How to install and run
## Requirements
- A computer or virtual machine running POSIX compatible operating systems. (Since we're using POSIX sockets & pthreads)
- GNU make.
- A version of GNU g++ compiler that supports c++11.

## Install and run
1. Clone or download the repository.
2. cd into the project directory and run `make` on terminal
3. Run `./EM <port-number>` on terminal. Port-number should be larger than 1024.
You should see the messsage `Listening on port <port-number>` on success.
4. Open your favorite browser and navigate to localhost:<port-number>
5. Voila ! You can now challenge EarthMover.

# File structure
- **compare/** : Contains a python script that can compare the strength of two different versions of EM.
- **server/** : HTTP Server related code.
- **neuralnetwork/** : Expiremental. Using neural networks to train EM.
- **gomoku/** : Contains the evaluate function & other game-specific code.
  - **src/** : front end code & resources.
  - filenames related to 'evaluator' : evaluates the score of the given chesstype.
  - filenames related to 'typetree" : classify the chess type (whether it's a live 3 or dead 4, etc.)
- **lib/** : Third party libraries.
- **networkmain.cpp** : The enrty point (`main` function).
- **ai.h, ai.cpp** : A high level interface to gametree's MCTS process.
- **gametree.h, gametree.cpp** : Performs the MCTS process.
- **node.h, node.cpp** : The component of MCTS tree.
- **Some other files...**
