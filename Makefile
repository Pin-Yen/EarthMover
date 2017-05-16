objects = obj/httpserver.o obj/ai.o obj/gametree.o obj/node.o\
		  obj/displayboard.o obj/virtualboard.o obj/point.o obj/evaluator.o\
		  obj/openingtree.o obj/typetree.o

# General compile options (e.g. Those that should be used in every compilation) should be placed here. 
generalFlags = -std=c++11

specificFlags =

# Default build
EM : mkdir $(objects) obj/networkmain.o
	 g++ -o EM $(objects) obj/networkmain.o $(generalFlags) -pthread -O3
EM : optimizeFlag = -O3

# Analyze build
analyze : specificFlags = -DANALYZE
analyze : optimizeFlag = -O3
analyze : mkdir $(objects) obj/log.o obj/main.o
	g++ -o EM obj/log.o obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

# Local-Debug build, based on analyze build. For debugging segfaults.
localdebug : specificFlags = -DDEBUG -g
localdebug : optimizeFlag = -O0 
localdebug : mkdir $(objects) obj/main.o obj/objectcounter.o
	g++ -o EM obj/objectcounter.o obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread
	
# Makes 'obj' directory to put the object files in 
mkdir : 
	if ! [ -d "obj" ]; then mkdir obj;fi

clean : 
	if [ -d "obj" ]; then rm -r obj; fi
	if [ -e "EM" ]; then rm EM; fi
#------------------------------------------------------------------------------
# Below are the dependencies of each obj file.
# You should modify it if you add any #include<.hpp>.
#------------------------------------------------------------------------------
obj/networkmain.o : httpserver.hpp ai.hpp gomoku/displayboard.hpp
	 g++ -c networkmain.cpp -o obj/networkmain.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/httpserver.o : ai.hpp gomoku/displayboard.hpp
	 g++ -c httpserver.cpp -o obj/httpserver.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/ai.o : gametree.hpp gomoku/displayboard.hpp ai.hpp
	 g++ -c ai.cpp -o obj/ai.o $(generalFlags) $(optimizeFlag) $(specificFlags) -pthread

obj/gametree.o : node.hpp gomoku/freestyle/virtualboard.hpp gametree.hpp gomoku/chesstype.hpp gomoku/status.hpp log.hpp
	 g++ -c gametree.cpp -o obj/gametree.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/node.o : gomoku/chesstype.hpp gomoku/chesstype.hpp node.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp\
             gametree.hpp objectcounter.hpp
	 g++ -c node.cpp -o obj/node.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/displayboard.o : gomoku/displayboard.hpp
	 g++ -c gomoku/displayboard.cpp -o obj/displayboard.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/virtualboard.o : gomoku/freestyle/virtualboard.hpp gomoku/chesstype.hpp gomoku/status.hpp\
					 gomoku/freestyle/point.hpp gomoku/freestyle/evaluator.hpp objectcounter.hpp
	 g++ -c gomoku/freestyle/virtualboard.cpp -o obj/virtualboard.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/point.o : gomoku/freestyle/point.hpp gomoku/status.hpp gomoku/chesstype.hpp gomoku/freestyle/virtualboard.hpp\
              objectcounter.hpp
	 g++ -c gomoku/freestyle/point.cpp -o obj/point.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/evaluator.o : gomoku/chesstype.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp\
				  gomoku/freestyle/point.hpp gomoku/freestyle/evaluator.hpp gomoku/freestyle/typetree.hpp\
				  gomoku/openingtree.hpp
	 g++ -c gomoku/freestyle/evaluator.cpp -o obj/evaluator.o $(generalFlags) $(optimizeFlag) $(specificFlags)
obj/openingtree.o : gomoku/status.hpp gomoku/freestyle/virtualboard.hpp gomoku/freestyle/point.hpp\
					gomoku/freestyle/evaluator.hpp gomoku/openingtree.hpp
	 g++ -c gomoku/openingtree.cpp -o obj/openingtree.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/typetree.o : gomoku/chesstype.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp\
				 gomoku/freestyle/evaluator.hpp gomoku/freestyle/typetree.hpp
	 g++ -c gomoku/freestyle/typetree.cpp -o obj/typetree.o $(generalFlags) $(optimizeFlag) $(specificFlags)

# The obj files below aren't required in every kind of build
obj/log.o : log.hpp
	g++ -c log.cpp -o obj/log.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/main.o : gomoku/chesstype.hpp gomoku/status.hpp gomoku/freestyle/virtualboard.hpp gomoku/displayboard.hpp\
			 gametree.hpp
	g++ -c main.cpp -o obj/main.o $(generalFlags) $(optimizeFlag) $(specificFlags)
obj/objectcounter.o : objectcounter.hpp
	g++ -c objectcounter.cpp -o obj/objectcounter.o $(generalFlags) $(optimizeFlag) $(specificFlags)
