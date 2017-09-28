OBJ = \
obj/gametree.o \
obj/gomokufreestyleevaluatorfreestyle.o \
obj/gomokufreestyletypetreefreestyle.o \
obj/gomokufreestylevirtualboardfreestyle.o \
obj/gomokurenju_basicevaluatorrenjubasic.o \
obj/gomokurenju_basictypetreerenjubasic.o \
obj/gomokurenju_basicvirtualboardrenjubasic.o \
obj/memorypool.o \
obj/node.o

OBJ_NET = \
obj/ai.o \
obj/networkmain.o \
obj/serverhttprequest.o \
obj/serverhttpresponse.o \
obj/serverhttpserver.o

OBJ_LOCAL = \
obj/gomokudisplayboard.o \
obj/main.o

GENERAL_FLAG = -std=c++11

EM : mkdir $(OBJ) $(OBJ_NET)
	g++ -o EM $(OBJ) $(OBJ_NET) $(GENERAL_FLAG) -pthread -O3
EM : OPT_FLAG = -O3

Debug : mkdir $(OBJ) $(OBJ_NET)
	g++ -o EM $(OBJ) $(OBJ_NET) $(GENERAL_FLAG) -pthread -O0 -g
Debug : OPT_FLAG = -O0 -g

Local : mkdir $(OBJ) $(OBJ_LOCAL)
	g++ -o EM $(OBJ) $(OBJ_LOCAL) $(GENERAL_FLAG) -pthread -O3
Local : OPT_FLAG = -O3

mkdir :
	if ! [ -d "obj" ]; then mkdir obj; fi

clean :
	if [ -d "obj" ]; then rm -r obj; fi
	if [ -e "EM" ]; then rm EM; fi

obj/ai.o: ai.cpp \
 ai.h \
 const.h \
 gametree.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/status.h \
 gomoku/virtualboardgomoku.h \
 lib/json.h \
 memorypool.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gametree.o: gametree.cpp \
 const.h \
 gametree.h \
 gomoku/chesstype.h \
 gomoku/status.h \
 lib/json.h \
 memorypool.h \
 node.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokudisplayboard.o: gomoku/displayboard.cpp \
 gomoku/displayboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokufreestyleevaluatorfreestyle.o: gomoku/freestyle/evaluatorfreestyle.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/typetreefreestyle.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/status.h \
 gomoku/typetree.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokufreestyletypetreefreestyle.o: gomoku/freestyle/typetreefreestyle.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/typetreefreestyle.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/status.h \
 gomoku/typetree.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokufreestylevirtualboardfreestyle.o: gomoku/freestyle/virtualboardfreestyle.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/freestyle/evaluatorfreestyle.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/status.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokurenju_basicevaluatorrenjubasic.o: gomoku/renju_basic/evaluatorrenjubasic.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/status.h \
 gomoku/typetree.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokurenju_basictypetreerenjubasic.o: gomoku/renju_basic/typetreerenjubasic.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/typetreerenjubasic.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/status.h \
 gomoku/typetree.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/gomokurenju_basicvirtualboardrenjubasic.o: gomoku/renju_basic/virtualboardrenjubasic.cpp \
 const.h \
 gomoku/chesstype.h \
 gomoku/evaluator.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/renju_basic/evaluatorrenjubasic.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/status.h \
 gomoku/virtualboardgomoku.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/main.o: main.cpp \
 const.h \
 gametree.h \
 gomoku/chesstype.h \
 gomoku/displayboard.h \
 gomoku/evaluator.h \
 gomoku/freestyle/virtualboardfreestyle.h \
 gomoku/openingtree.h \
 gomoku/point.h \
 gomoku/renju_basic/virtualboardrenjubasic.h \
 gomoku/status.h \
 gomoku/virtualboardgomoku.h \
 lib/json.h \
 memorypool.h \
 timer.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/memorypool.o: memorypool.cpp \
 memorypool.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/networkmain.o: networkmain.cpp \
 ai.h \
 const.h \
 gametree.h \
 lib/json.h \
 memorypool.h \
 server/httprequest.h \
 server/httpresponse.h \
 server/httpserver.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/node.o: node.cpp \
 const.h \
 gametree.h \
 gomoku/chesstype.h \
 gomoku/status.h \
 lib/json.h \
 memorypool.h \
 node.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/serverhttprequest.o: server/httprequest.cpp \
 server/httprequest.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/serverhttpresponse.o: server/httpresponse.cpp \
 server/httpresponse.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

obj/serverhttpserver.o: server/httpserver.cpp \
 ai.h \
 const.h \
 gametree.h \
 lib/json.h \
 memorypool.h \
 server/httprequest.h \
 server/httpresponse.h \
 server/httpserver.h \
 virtualboard.h
	g++ -c $(GENERAL_FLAG) $(OPT_FLAG) $< -o $@

