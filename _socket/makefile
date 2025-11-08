select=-pthread -std=c++11 -g
.PHONY:all
all:Server Client
Server:tcpServer.cpp
	g++ -o $@ $^ ${select}
Client:tcpClient.cpp
	g++ -o $@ $^ ${select}
.PHONY:clean
clean:
	rm -rf Server Client
