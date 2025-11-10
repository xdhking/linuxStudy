slt=-ljsoncpp -lpthread -std=c++11 -g
d=-DMYSELF
.PHONY:all
all:server client
server:calServer.cpp
	g++ -o $@ $^ ${slt} ${d}
client:calClient.cpp
	g++ -o $@ $^ ${slt} ${d}
.PHONY:clean
clean:
	rm -rf server client