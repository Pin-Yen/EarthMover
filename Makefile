
################# ONLY THE DEFAULT BUILD WILL WORK #################

objects = obj/displayboard.o obj/evaluatorfreestyle.o obj/evaluatorrenjubasic.o\
		  obj/typetreefreestyle.o obj/typetreerenjubasic.o obj/gametree.o\
		  obj/node.o

objects_network = obj/ai.o obj/httpserver.o obj/networkmain.o

# General compile options (e.g. Those that should be used in every compilation) should be placed here. 
generalFlags = -std=c++11

specificFlags =

# Default build
EM : mkdir $(objects) $(objects_network)
	 g++ -o EM $(objects) $(objects_network) $(generalFlags) -pthread -O3
EM : optimizeFlag = -O3

# Analyze build
analyze : specificFlags = -DANALYZE
analyze : optimizeFlag = -O3
analyze : mkdir $(objects) obj/log.o obj/main.o
	g++ -o EM obj/log.o obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

# Local-Debug build. For debugging segfaults.
localdebug : specificFlags = -DDEBUG -g
localdebug : optimizeFlag = -O0 
localdebug : mkdir $(objects) obj/main.o obj/objectcounter.o
	g++ -o EM obj/objectcounter.o obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

#Local-Time build. For performance evaluation.
localtime : specificFlags = -DTIME
localtime : optimizeFlag = -O3
localtime : mkdir $(objects) obj/main.o
	g++ -o EM obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

# Makes 'obj' directory to put the object files in 
mkdir : 
	if ! [ -d "obj" ]; then mkdir obj; fi

clean : 
	if [ -d "obj" ]; then rm -r obj; fi
	if [ -e "EM" ]; then rm EM; fi
#------------------------------------------------------------------------------
# Below are the dependencies of each obj file.
# You should modify it if you add any #include ".hpp".
#------------------------------------------------------------------------------
obj/displayboard.o: gomoku/displayboard.cpp gomoku/displayboard.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/evaluatorfreestyle.o: gomoku/freestyle/evaluatorfreestyle.cpp \
 gomoku/freestyle/../chesstype.hpp gomoku/freestyle/../status.hpp \
 gomoku/freestyle/../../virtualboard.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp \
 gomoku/freestyle/../../virtualboard.hpp \
 gomoku/freestyle/../chesstype.hpp gomoku/freestyle/../status.hpp \
 gomoku/freestyle/../evaluator.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp gomoku/freestyle/../point.hpp \
 gomoku/freestyle/../openingtree.hpp \
 gomoku/freestyle/virtualboardfreestyle.hpp \
 gomoku/freestyle/evaluatorfreestyle.hpp \
 gomoku/freestyle/../evaluator.hpp gomoku/freestyle/../typetree.hpp \
 gomoku/freestyle/typetreefreestyle.hpp gomoku/freestyle/../point.hpp \
 gomoku/freestyle/../openingtree.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/typetreefreestyle.o: gomoku/freestyle/typetreefreestyle.cpp \
 gomoku/freestyle/../chesstype.hpp gomoku/freestyle/../status.hpp \
 gomoku/freestyle/../../virtualboard.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp \
 gomoku/freestyle/../../virtualboard.hpp \
 gomoku/freestyle/../chesstype.hpp gomoku/freestyle/../status.hpp \
 gomoku/freestyle/../evaluator.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp gomoku/freestyle/../point.hpp \
 gomoku/freestyle/../openingtree.hpp \
 gomoku/freestyle/virtualboardfreestyle.hpp \
 gomoku/freestyle/evaluatorfreestyle.hpp \
 gomoku/freestyle/../evaluator.hpp gomoku/freestyle/../typetree.hpp \
 gomoku/freestyle/typetreefreestyle.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/evaluatorrenjubasic.o: gomoku/renju_basic/evaluatorrenjubasic.cpp \
 gomoku/renju_basic/../chesstype.hpp gomoku/renju_basic/../status.hpp \
 gomoku/renju_basic/virtualboardrenjubasic.hpp \
 gomoku/renju_basic/../virtualboardgomoku.hpp \
 gomoku/renju_basic/../../virtualboard.hpp \
 gomoku/renju_basic/../chesstype.hpp gomoku/renju_basic/../status.hpp \
 gomoku/renju_basic/../evaluator.hpp \
 gomoku/renju_basic/../virtualboardgomoku.hpp \
 gomoku/renju_basic/../point.hpp gomoku/renju_basic/../openingtree.hpp \
 gomoku/renju_basic/evaluatorrenjubasic.hpp \
 gomoku/renju_basic/typetreerenjubasic.hpp \
 gomoku/renju_basic/../typetree.hpp gomoku/renju_basic/../point.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/typetreerenjubasic.o: gomoku/renju_basic/typetreerenjubasic.cpp \
 gomoku/renju_basic/../chesstype.hpp gomoku/renju_basic/../status.hpp \
 gomoku/renju_basic/virtualboardrenjubasic.hpp \
 gomoku/renju_basic/../virtualboardgomoku.hpp \
 gomoku/renju_basic/../../virtualboard.hpp \
 gomoku/renju_basic/../chesstype.hpp gomoku/renju_basic/../status.hpp \
 gomoku/renju_basic/../evaluator.hpp \
 gomoku/renju_basic/../virtualboardgomoku.hpp \
 gomoku/renju_basic/../point.hpp gomoku/renju_basic/../openingtree.hpp \
 gomoku/renju_basic/evaluatorrenjubasic.hpp \
 gomoku/renju_basic/typetreerenjubasic.hpp \
 gomoku/renju_basic/../typetree.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/gametree.o: gametree.cpp gomoku/chesstype.hpp gomoku/status.hpp \
 virtualboard.hpp gametree.hpp node.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/node.o: node.cpp gomoku/chesstype.hpp gomoku/status.hpp virtualboard.hpp \
 gametree.hpp node.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

# The obj files below aren't required in every kind of build, so they aren't included in the 'objects' variable.

obj/ai.o: ai.cpp ai.hpp gomoku/displayboard.hpp gametree.hpp virtualboard.hpp \
 gomoku/freestyle/virtualboardfreestyle.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp \
 gomoku/freestyle/../../virtualboard.hpp \
 gomoku/freestyle/../chesstype.hpp gomoku/freestyle/../status.hpp \
 gomoku/freestyle/../evaluator.hpp \
 gomoku/freestyle/../virtualboardgomoku.hpp gomoku/freestyle/../point.hpp \
 gomoku/freestyle/../openingtree.hpp \
 gomoku/freestyle/evaluatorfreestyle.hpp \
 gomoku/freestyle/../evaluator.hpp gomoku/freestyle/../typetree.hpp \
 gomoku/freestyle/typetreefreestyle.hpp \
 gomoku/renju_basic/virtualboardrenjubasic.hpp \
 gomoku/renju_basic/../virtualboardgomoku.hpp \
 gomoku/renju_basic/evaluatorrenjubasic.hpp \
 gomoku/renju_basic/typetreerenjubasic.hpp \
 gomoku/renju_basic/../typetree.hpp const.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread $< -o $@

obj/httpserver.o: httpserver.cpp ai.hpp gomoku/displayboard.hpp gametree.hpp \
 virtualboard.hpp const.hpp httpserver.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/networkmain.o: networkmain.cpp httpserver.hpp ai.hpp \
 gomoku/displayboard.hpp gametree.hpp virtualboard.hpp
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/log.o : log.hpp
	g++ -c log.cpp -o obj/log.o $(generalFlags) $(optimizeFlag) $(specificFlags)
