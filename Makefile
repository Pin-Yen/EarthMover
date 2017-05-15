objects = obj/networkmain.o obj/httpserver.o obj/ai.o obj/gametree.o obj/node.o\
		  obj/displayboard.o obj/virtualboard.o obj/point.o obj/evaluator.o\
		  obj/openingtree.o obj/typetree.o

# General compile options (e.g. Those that should be used in every kind of build) should be placed here. 
generalFlags = -std=c++11 -pthread

# Default build
EM : mkdir $(objects) 
	 g++ -o EM $(objects) $(generalFlags) -O3
EM : optimizeFlag = -O3

# Makes 'obj' directory to put the object files in 
mkdir : 
	if ! [ -d "obj" ]; then mkdir obj;fi


#------------------------------------------------------------------------------
# Below are the dependencies of each obj file.
# You should modify it if you add any #include<.hpp>.
#------------------------------------------------------------------------------
obj/networkmain.o : httpserver.hpp ai.hpp gomoku/displayboard.hpp
	 g++ -c networkmain.cpp -o obj/networkmain.o $(generalFlags) $(optimizeFlag)

obj/httpserver.o : ai.hpp gomoku/displayboard.hpp
	 g++ -c httpserver.cpp -o obj/httpserver.o $(generalFlags) $(optimizeFlag)

obj/ai.o : gametree.hpp gomoku/displayboard.hpp ai.hpp
	 g++ -c ai.cpp -o obj/ai.o -pthread $(generalFlags) $(optimizeFlag)

obj/gametree.o : node.hpp gomoku/freestyle/virtualboard.hpp gametree.hpp gomoku/chesstype.hpp gomoku/status.hpp
	 g++ -c gametree.cpp -o obj/gametree.o $(generalFlags) $(optimizeFlag)

obj/node.o : gomoku/chesstype.hpp gomoku/chesstype.hpp node.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp gametree.hpp
	 g++ -c node.cpp -o obj/node.o $(generalFlags) $(optimizeFlag)

obj/displayboard.o : gomoku/displayboard.hpp
	 g++ -c gomoku/displayboard.cpp -o obj/displayboard.o $(generalFlags) $(optimizeFlag)

obj/virtualboard.o : gomoku/freestyle/virtualboard.hpp gomoku/chesstype.hpp gomoku/status.hpp\
					 gomoku/freestyle/point.hpp gomoku/freestyle/evaluator.hpp
	 g++ -c gomoku/freestyle/virtualboard.cpp -o obj/virtualboard.o $(generalFlags) $(optimizeFlag)

obj/point.o : gomoku/freestyle/point.hpp gomoku/status.hpp gomoku/chesstype.hpp gomoku/freestyle/virtualboard.hpp
	 g++ -c gomoku/freestyle/point.cpp -o obj/point.o $(generalFlags) $(optimizeFlag)

obj/evaluator.o : gomoku/chesstype.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp\
				  gomoku/freestyle/point.hpp gomoku/freestyle/evaluator.hpp gomoku/freestyle/typetree.hpp\
				  gomoku/openingtree.hpp
	 g++ -c gomoku/freestyle/evaluator.cpp -o obj/evaluator.o $(generalFlags) $(optimizeFlag)
obj/openingtree.o : gomoku/status.hpp gomoku/freestyle/virtualboard.hpp gomoku/freestyle/point.hpp\
					gomoku/freestyle/evaluator.hpp gomoku/openingtree.hpp
	 g++ -c gomoku/openingtree.cpp -o obj/openingtree.o $(generalFlags) $(optimizeFlag)

obj/typetree.o : gomoku/chesstype.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp\
				 gomoku/freestyle/evaluator.hpp gomoku/freestyle/typetree.hpp
	 g++ -c gomoku/freestyle/typetree.cpp -o obj/typetree.o $(generalFlags) $(optimizeFlag)
