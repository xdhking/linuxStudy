.PHONY:all
all:shm_client shm_server
shm_client:shm_client.cpp
	g++ -o $@ $^ -std=c++11
shm_server:shm_server.cpp
	g++ -o $@ $^ -std=c++11
.pHONY:clean
clean:
	rm -rf shm_client shm_server