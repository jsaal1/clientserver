CXX      = g++
CPPFLAGS = -I include
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic-errors -g
LDFLAGS  = -L lib
LDLIBS   = -lclientserver

LIB      = lib/libclientserver.a
LIB_SRC  = src/connection.cc src/server.cc
LIB_OBJ  = $(patsubst src/%.cc,build/make/%.o,$(LIB_SRC))

SRC      = src/messagehandler.cc src/newsserver.cc
SRC_MEM  = $(SRC) src/inmemorydatabase.cc src/server_mem_main.cc
SRC_DISK = $(SRC) src/diskdatabase.cc src/server_disk_main.cc
SRC_CLI  = src/messagehandler.cc src/client_main.cc

BINS = bin/server_mem bin/server_disk bin/client_main

.PHONY: all clean

all: $(BINS)

# Communication library (Connection, Server)
build/make/%.o: src/%.cc
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(LIB): $(LIB_OBJ)
	@mkdir -p $(@D)
	ar rcs $@ $^

# Executables
bin/server_mem: $(SRC_MEM) $(LIB)
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(filter %.cc,$^) $(LDFLAGS) $(LDLIBS) -o $@

bin/server_disk: $(SRC_DISK) $(LIB)
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(filter %.cc,$^) $(LDFLAGS) $(LDLIBS) -o $@

bin/client_main: $(SRC_CLI) $(LIB)
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(filter %.cc,$^) $(LDFLAGS) $(LDLIBS) -o $@

clean:
	rm -rf bin lib build/make
