# News Server (C++17)

A Usenet-style news system: a server that stores newsgroups and articles, and an interactive terminal
client that talks to it over TCP using a compact binary protocol. Built as the project for the C++
Programming course (EDAF50) at LTH, Lund University.

## What it does

The client can list, create and delete newsgroups, and list, create, read and delete articles in a group.
Two server variants share the same request handling and differ only in storage:

- `server_mem` keeps everything in memory, so data is lost when it stops.
- `server_disk` stores each newsgroup as a directory and each article as a file, so data survives restarts.

## Design

```
client_main ──► MessageHandler ══ TCP ══ MessageHandler ◄── NewsServer ──► Database
                                                                             ├── InMemoryDatabase
                                                                             └── DiskDatabase
```

- **`MessageHandler`** encodes and decodes the wire format: one-byte command and answer codes,
  big-endian 32-bit integers and length-prefixed strings. Both the client and the server use it, so the
  protocol is implemented once.
- **`NewsServer`** reads one request at a time, validates that it is well formed (anything unexpected
  closes the connection), calls the database and sends the reply, including error codes for missing
  newsgroups or articles.
- **`Database`** is an abstract interface with two implementations. `InMemoryDatabase` uses ordered maps;
  `DiskDatabase` uses `std::filesystem`, with id counters stored on disk so ids are never reused after
  a deletion.

`Connection`, `Server`, `ConnectionClosedException` and the protocol codes in `protocol.h` were provided by
the course. Everything else (`MessageHandler`, `NewsServer`, the `Database` interface and both
implementations, and the client) is my own work.

## Build

Requires a C++17 compiler. Either build system works.

**Make**

```sh
make            # builds lib/libclientserver.a, then bin/server_mem, bin/server_disk, bin/client_main
make clean
```

**CMake**

```sh
cmake -B build
cmake --build build
```

The binaries end up in `build/src/`. Debug builds (`-DCMAKE_BUILD_TYPE=Debug`) enable AddressSanitizer and
UndefinedBehaviorSanitizer.

## Run

Start a server in one terminal:

```sh
bin/server_mem 7777                 # in-memory
bin/server_disk 7777 ./db           # stored under ./db
```

Connect from another:

```sh
bin/client_main localhost 7777
```

```
1) List newsgroups
2) Create newsgroup
3) Delete newsgroup
4) List articles by newsgroup
5) Create article
6) Delete article
7) Get article
8) Clear screen
.) Quit
```

With CMake, use `build/src/...` instead of `bin/...`.
