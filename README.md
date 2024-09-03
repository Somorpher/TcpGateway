# TcpGateway: A C++ TCP Socket Library
### Overview


[![MIT License](https://img.shields.io/badge/License-MIT-orange.svg)](https://github.com/Somorpher/Diskrypt/blob/main/LICENSE) 

The library provides a robust and easy-to-use interface for creating and managing TCP socket connections in C++. It is designed to facilitate both server and client functionalities, allowing developers to establish connections, send and receive data, and handle multiple connections efficiently. This library abstracts the complexities of socket programming, making it accessible for both novice and experienced developers.
Features

Simplifies TCP socket programming by providing an intuitive interface for creating and managing both client and server connections. Key features include socket initialization, server creation, client connection, data transmission, and error handling. It supports multiple connections with configurable limits, ensures thread safety, and offers methods for checking connection status and managing socket states.

### Installation

To use the TcpInitializer library, include the header file in your C++ project:

```cpp
#include "TcpGateway/unix-g4tcpp_v0_0_1.hpp"
```
Usage
Initializing a Socket

To initialize a TCP socket, simply call the Init method:

```cpp
TcpInitializer::Socket socket;
socket.Init();
```
Creating a TCP Server

To create a TCP server, use the TcpServer method, providing the desired address and port:

```cpp
socket.TcpServer("127.0.0.1", 8080);
```
You can also create a server on a default IP address:

```cpp
socket.TcpServer(8080);
```
Connecting to a Server

To connect to a remote server, use the Connect method:

```cpp
bool isConnected = socket.Connect("192.168.1.1", 8080);
```
Sending Data

To send data over an established connection, use the Send method:

```cpp
bool success = socket.Send("Hello, Server!");
```
Receiving Data

To read incoming data from the socket, use the Read method:

```cpp
TcpInitializer::TcpIntercept request = socket.Read();
std::cout << "Received: " << request.raw_bytes << std::endl;
```
Handling New Connections

To accept new incoming connections, use the NewRequest method:

```cpp
__socket newConnection = socket.NewRequest();
```
Closing a Connection

To close a socket connection, use the Close method:

```cpp
socket.Close(socket.GetSocket());
```
Managing Connection Limits

You can set the maximum number of allowed connections with:

```cpp
socket.SetMaxConnections(100);
```
Checking Connection Status

To check if the socket is connected, use:

```cpp
bool connected = socket.IsConnected();
```
Error Handling

The library provides built-in error handling. If an operation fails, an exception is thrown with a descriptive error message. You can catch these exceptions to handle errors gracefully in your application.

```cpp
try {
    socket.TcpServer("127.0.0.1", 8080);
} catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```
