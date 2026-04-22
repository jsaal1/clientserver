# News System

## Build

```bash
make all
```

This produces three executables in `bin/`:
- `bin/server_mem` — in-memory server
- `bin/server_disk` — disk-based server
- `bin/client_main` — news client

## Run

Start the in-memory server:
```bash
./bin/server_mem <port>
```

Start the disk-based server (data persists between runs):
```bash
./bin/server_disk <port> <database-directory>
```

Start the client (in a separate terminal):
```bash
./bin/client_main <host> <port>
```

Example:
```bash
./bin/server_disk 1234 db
./bin/client_main localhost 1234
```

## Clean

```bash
make clean
```
