.PHONY:all
all:mytest mysignal
mysignal:mysignal.cpp
	g++ -o $@ $^ -g -std=c++11
mytest:mytest.cpp
	g++ -o $@ $^ -g -std=c++11
.PHONY:clean
clean:
	rm -rf mysignal mytest