# CPP Project, Communication Classes

The clientserver directory contains the communication classes Connection and Server,
and the auxiliary classes ConnectionClosedException and Protocol.

## Building with make

```
make
```

Builds `libclientserver.a` from `src/connection.cc` and `src/server.cc`, then compiles
`server_mem`, `server_disk`, and `client_main` into `bin/`.

For cleaning: `make clean`.

## Building with CMake (recommended)

```
mkdir build
cd build
cmake ..
cmake --build .
```

Built binaries will be in `build/src/`: `server_mem`, `server_disk`, and `client_main`.

To install (e.g. under /tmp):

```
cmake -DCMAKE_INSTALL_PREFIX=/tmp ..
cmake --build .
cmake --install .
```

## Running

Open two terminal windows.

Start the server (in-memory or disk-backed):

```
build/src/server_mem <port>
build/src/server_disk <port>
```

Start the client:

```
build/src/client_main <server> <port>
```
