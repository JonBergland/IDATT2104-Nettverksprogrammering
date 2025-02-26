For å starte serveren:
```bash
g++ -I C:/msys64/mingw64/lib/asio-1.30.2/include -I C:/msys64/mingw64/include -L C:/msys64/mingw64/lib -o server main.cpp -lws2_32 -lssl -lcrypto
```

```bash
./server.exe 8080
```

For å starte UDP-klienten:
```bash
g++ -o udp_client udp_client.cpp -lws2_32
```