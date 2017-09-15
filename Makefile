
objects = obj/displayboard.o obj/evaluatorfreestyle.o obj/evaluatorrenjubasic.o\
			obj/typetreefreestyle.o obj/typetreerenjubasic.o obj/gametree.o\
			obj/node.o obj/memorypool.o

objects_network = obj/ai.o obj/httpserver.o obj/httprequest.o obj/httpresponse.o obj/networkmain.o
objects_local = obj/main.o

# General compile options (e.g. Those that should be used in every compilation) should be placed here.
generalFlags = -std=c++11

specificFlags =

# Default build
EM : mkdir $(objects) $(objects_network)
	 g++ -o EM $(objects) $(objects_network) $(generalFlags) -pthread -O3
EM : optimizeFlag = -O3

# Network Debug
NetworkDebug : mkdir $(objects) $(objects_network)
	 g++ -o EM $(objects) $(objects_network) $(generalFlags) -pthread -O0 -g
NetworkDebug : optimizeFlag = -O0 -g

# Local-Debug build. For debugging segfaults.
# localdebug : specificFlags = -DDEBUG -g
# localdebug : optimizeFlag = -O0
# localdebug : mkdir $(objects) obj/main.o obj/objectcounter.o
	# g++ -o EM obj/objectcounter.o obj/main.o $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

#Local-Time build. For performance evaluation.
localtime : specificFlags = -DTIME
localtime : optimizeFlag = -O3
localtime : mkdir $(objects) $(objects_local)
	g++ -o EM $(objects_local) $(objects) $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread

# Makes 'obj' directory to put the object files in
mkdir :
	if ! [ -d "obj" ]; then mkdir obj; fi

clean :
	if [ -d "obj" ]; then rm -r obj; fi
	if [ -e "EM" ]; then rm EM; fi
#------------------------------------------------------------------------------
# Below are the dependencies of each obj file.
# You should modify it if you add any #include ".h".
#------------------------------------------------------------------------------
obj/displayboard.o: gomoku/displayboard.cpp gomoku/displayboard.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/evaluatorfreestyle.o: gomoku/freestyle/evaluatorfreestyle.cpp \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../virtualboardgomoku.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../evaluator.h \
 gomoku/freestyle/../virtualboardgomoku.h gomoku/freestyle/../point.h \
 gomoku/freestyle/../openingtree.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/../evaluator.h gomoku/freestyle/../typetree.h \
 gomoku/freestyle/typetreefreestyle.h gomoku/freestyle/../point.h \
 gomoku/freestyle/../openingtree.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/typetreefreestyle.o: gomoku/freestyle/typetreefreestyle.cpp \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../virtualboardgomoku.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../evaluator.h \
 gomoku/freestyle/../virtualboardgomoku.h gomoku/freestyle/../point.h \
 gomoku/freestyle/../openingtree.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/../evaluator.h gomoku/freestyle/../typetree.h \
 gomoku/freestyle/typetreefreestyle.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/evaluatorrenjubasic.o: gomoku/renju_basic/evaluatorrenjubasic.cpp \
 gomoku/renju_basic/../chesstype.h gomoku/renju_basic/../status.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/../../virtualboard.h \
 gomoku/renju_basic/../chesstype.h gomoku/renju_basic/../status.h \
 gomoku/renju_basic/../evaluator.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/../point.h gomoku/renju_basic/../openingtree.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/../typetree.h gomoku/renju_basic/../point.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/typetreerenjubasic.o: gomoku/renju_basic/typetreerenjubasic.cpp \
 gomoku/renju_basic/../chesstype.h gomoku/renju_basic/../status.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/../../virtualboard.h \
 gomoku/renju_basic/../chesstype.h gomoku/renju_basic/../status.h \
 gomoku/renju_basic/../evaluator.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/../point.h gomoku/renju_basic/../openingtree.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/../typetree.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/memorypool.o: memorypool.cpp memorypool.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/gametree.o: gametree.cpp gomoku/chesstype.h gomoku/status.h \
 virtualboard.h gametree.h node.h memorypool.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/node.o: node.cpp gomoku/chesstype.h gomoku/status.h virtualboard.h \
 gametree.h node.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

# The obj files below aren't required in every kind of build, so they aren't included in the 'objects' variable.

obj/ai.o: ai.cpp ai.h gomoku/displayboard.h gametree.h virtualboard.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/freestyle/../virtualboardgomoku.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../evaluator.h \
 gomoku/freestyle/../virtualboardgomoku.h gomoku/freestyle/../point.h \
 gomoku/freestyle/../openingtree.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/../evaluator.h gomoku/freestyle/../typetree.h \
 gomoku/freestyle/typetreefreestyle.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/../typetree.h const.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) -pthread $< -o $@

obj/httpserver.o: server/httpserver.cpp server/httpserver.h server/httprequest.h \
 server/httpresponse.h lib/json.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/httpresponse.o: server/httpresponse.cpp server/httpresponse.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/httprequest.o: server/httprequest.cpp server/httprequest.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/networkmain.o: networkmain.cpp server/httpserver.h ai.h \
 gomoku/displayboard.h gametree.h virtualboard.h\
 lib/json.h
	g++ -c $(generalFlags) $(specificFlags) $(optimizeFlag) $< -o $@

obj/log.o : log.h
	g++ -c log.cpp -o obj/log.o $(generalFlags) $(optimizeFlag) $(specificFlags)

obj/main.o: main.cpp gametree.h virtualboard.h gomoku/chesstype.h \
 gomoku/displayboard.h gomoku/point.h gomoku/chesstype.h \
 gomoku/status.h gomoku/virtualboardgomoku.h \
 gomoku/../virtualboard.h gomoku/evaluator.h gomoku/point.h \
 gomoku/openingtree.h gomoku/status.h gomoku/virtualboardgomoku.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/freestyle/../virtualboardgomoku.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/../evaluator.h gomoku/freestyle/../typetree.h \
 gomoku/freestyle/../chesstype.h gomoku/freestyle/../status.h \
 gomoku/freestyle/../../virtualboard.h \
 gomoku/freestyle/../virtualboardgomoku.h \
 gomoku/freestyle/typetreefreestyle.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/renju_basic/../virtualboardgomoku.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/../typetree.h \
 timer.h
	g++ -c main.cpp -o obj/main.o $(generalFlags) $(optimizeFlag) $(specificFlags)

