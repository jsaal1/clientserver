CXX      = g++
CPPFLAGS = -I include
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic-errors -g
LDFLAGS  = -L lib -lclientserver

SRC = src/messagehandler.cc \
      src/newsserver.cc

SRC_MEM  = $(SRC) src/inmemorydatabase.cc
SRC_DISK = $(SRC) src/diskdatabase.cc

all: bin/server_mem bin/server_disk bin/client_main

bin/server_mem: $(SRC_MEM) src/server_mem_main.cc
	mkdir -p bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

bin/server_disk: $(SRC_DISK) src/server_disk_main.cc
	mkdir -p bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

bin/client_main: $(SRC) src/client_main.cc
	mkdir -p bin
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

install: all

clean:
	rm -rf bin/*
