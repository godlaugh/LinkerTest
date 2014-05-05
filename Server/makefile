cc=g++
exe=ConnServer
obj=ConnServer.o Globals.o Net_Utils.o Utils.o Obj_Server.o Obj_Msg.o
header=Globals.h Net_Utils.h Utils.h Obj_Server.h Obj_Msg.h
libs=protobuf -lpthread -lcurl -lhiredis -llua -lyaml-cpp
$(exe):$(obj)
	$(cc) -g $(obj) -o $(exe) -l $(libs)
ConnServer.o:ConnServer.cpp $(header)
	$(cc) -g -c ConnServer.cpp
Globals.o:Globals.cpp $(header)
	$(cc) -g -c Globals.cpp
Net_Utils.o:Net_Utils.cpp $(header)
	$(cc) -g -c Net_Utils.cpp
Utils.o:Utils.cpp $(header)
	$(cc) -g -c Utils.cpp
Obj_Server.o:Obj_Server.cpp $(header)
	$(cc) -g -c Obj_Server.cpp
Obj_Msg.o:Obj_Msg.cpp $(header)
	$(cc) -g -c Obj_Msg.cpp
clean:
	rm -rf *.o $(exe)
