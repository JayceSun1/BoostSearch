PARSER=parser
DUG = debug
HTTP_SERVER = http_server
cc=g++

.PHONY:all
all:$(PARSER) $(DUG) $(HTTP_SERVER)

$(PARSER):parcer.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11
$(DUG):debug.cc
	$(cc) -o $@ $^ -ljsoncpp -std=c++11
$(HTTP_SERVER):http_server.cc 
	$(cc) -o $@ $^ -lpthread -ljsoncpp -std=c++11

.PHONY:clean
clean:
	rm -f $(PARSER) $(DUG) $(HTTP_SERVER)
