#ifndef UNIX_GT4TCPP_V0_0_1_HPP

#ifndef UNIX_TCP_INITIALIZER_VERSION
#include "unix-g4tcpp_v0_0_1.hpp"

using namespace TcpInitializer::_t; // types

using __self__ = TcpInitializer::Socket; // static reference object alias

const decltype(std::string::npos) __noindex = std::string::npos;

/**
 * @brief Initializes the TCP socket channel, required for both client and server.
 * 
 * This function creates a socket using the TCP protocol.
 */
void TcpInitializer::Socket::Init(void) noexcept { 
    *__self__::_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
};

/**
 * @brief Creates a TCP server on the specified address and port.
 * 
 * @param _address The remote address to bind the server to.
 * @param _port The port number to listen on.
 * @throws std::runtime_error If address validation fails or socket creation fails.
 */
void TcpInitializer::Socket::CreateTcpServer(const t_strw _address, const t_u16 _port) {
    try {
        if (!__self__::_AddressValidate(_address, _port)) {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Arg Eval failure"));
        }

        __self__::_ip_address = _address;
        __self__::_port = _port;

        if (__self__::_socket.get() == nullptr || *__self__::_socket < 0) {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Socket failure"));
        }
        __self__::_tcp_state = TcpState::OPEN;

        __self__::Log("Tcp Channel: ", __self__::_ip_address, ":", __self__::_port, '\n');

        __self__::_AddressReuse();

        if (__self__::_TcpBind()) {
            if (__self__::_TcpListen()) {
                __self__::_tcp_state = TcpState::LISTENING;
            }
        }
    } catch (const t_except &e) {
        __self__::_ExceptionHandle(e.what());
    }
};

/**
 * @brief Creates a TCP server on the default address and specified port.
 * 
 * @param _port The port number to listen on.
 */
void TcpInitializer::Socket::CreateTcpServer(const t_u16 _port) { 
    __self__::CreateTcpServer(DEFAULT_IP_ADDRESS, _port); 
};

/**
 * @brief Connects to a server on the specified address and port.
 * 
 * @param _address The remote address to connect to.
 * @param _port The port number to connect to.
 * @returns true if the connection was successful, false otherwise.
 */
bool TcpInitializer::Socket::Connect(const t_strw _address, const t_u16 _port) {
    bool _r{false};
    std::thread([&]() -> void { __self__::_Connect<bool &>(_address, _port, _r, false); }).join();
    return _r;
};

/**
 * @brief Connects to a server on the specified address and port, with an option to throw exceptions.
 * 
 * @param _address The remote address to connect to.
 * @param _port The port number to connect to.
 * @param _throw If true, exceptions will be thrown on errors.
 * @returns A ClientTcpConnection object representing the new connection.
 */
const TcpInitializer::ClientTcpConnection TcpInitializer::Socket::Connect(const t_strw _address, const t_u16 _port, const bool _throw) {
    TcpInitializer::ClientTcpConnection tcp_new;
    std::thread([&]() -> void { __self__::_Connect<TcpInitializer::ClientTcpConnection &>(_address, _port, tcp_new, _throw); }).join();
    return tcp_new;
};

/**
 * @brief Accepts a new TCP request on the specified socket.
 * 
 * @param _sock Pointer to the socket to accept requests from.
 * @returns A new TCP socket connection.
 * @throws std::invalid_argument If the socket is not connected.
 */
t_sock TcpInitializer::Socket::AcceptTcpRequest(t_sock *__restrict__ _sock) {
    if (!__self__::IsConnected())
        return -1;
    t_sock sock_digest;
    std::thread(__self__::_Accept, _sock, &sock_digest).join();
    return sock_digest;
};

/**
 * @brief Accepts a new TCP request on the internal socket.
 * 
 * @returns A new TCP socket connection.
 * @throws std::invalid_argument If the socket is not connected.
 */
t_sock TcpInitializer::Socket::AcceptTcpRequest(void) {
    if (!__self__::IsConnected())
        return -1;
    t_sock sock_digest;
    std::thread(__self__::_Accept, __self__::_socket.get(), &sock_digest).join();
    return sock_digest;
};

/**
 * @brief Sends data over the internal TCP socket.
 * 
 * @param _buffer The buffer to send over the TCP connection.
 * @returns true if the buffer was sent successfully, false otherwise.
 */
bool TcpInitializer::Socket::Send(const t_strw _buffer) noexcept { 
    return __self__::Send(__self__::_socket.get(), _buffer); 
};

/**
 * @brief Sends data over the specified TCP socket.
 * 
 * @param _sock Pointer to the socket to send data over.
 * @param _buffer The buffer to send over the TCP connection.
 * @returns true if the buffer was sent successfully, false otherwise.
 */
bool TcpInitializer::Socket::Send(const t_sock *__restrict__ _sock, const t_strw _buffer) noexcept {
    if (_sock == nullptr || *_sock <= 0)
        return false;
    return send(*_sock, _buffer.data(), _buffer.length(), 0) > 0;
};

/**
 * @brief Reads incoming TCP requests from the internal socket.
 * 
 * @returns A TcpIntercept object containing the intercepted TCP request.
 */
TcpInitializer::TcpIntercept TcpInitializer::Socket::Read(void) { 
    return __self__::Read(__self__::_socket.get()); 
};

/**
 * @brief Reads incoming TCP requests and stores the payload in the provided string reference.
 * 
 * @param sink_frame Reference to the string used to store the result of the socket read operation.
 */
void TcpInitializer::Socket::Read(t_str &sink_frame) {
    TcpInitializer::TcpIntercept tcp_request(__self__::Read(__self__::_socket.get()));
    sink_frame = tcp_request.block_size > 0 ? tcp_request.raw_bytes : "";
};

/**
 * @brief Reads TCP requests from the specified socket and stores the payload in the provided string reference.
 * 
 * @param _sock Pointer to the socket to read requests from.
 * @param sink_frame Reference to the string used to store the result of the socket read operation.
 */
void TcpInitializer::Socket::Read(t_sock *__restrict__ _sock, t_str &sink_frame) {
    TcpInitializer::TcpIntercept tcp_request(__self__::Read(_sock));
    sink_frame = tcp_request.block_size > 0 ? tcp_request.raw_bytes : "";
};

/**
 * @brief Reads TCP requests from the specified socket.
 * 
 * @param _sock Pointer to the socket to read requests from.
 * @returns A TcpIntercept object containing the intercepted TCP request.
 * @throws std::invalid_argument If the socket is invalid or not connected.
 */
TcpInitializer::TcpIntercept TcpInitializer::Socket::Read(t_sock *__restrict__ _sock) {
    TcpInitializer::TcpIntercept tcp_request;
    if (_sock == nullptr || *_sock <= 0 || !__self__::IsConnected())
        throw std::invalid_argument("invalid socket state");
    std::thread([&]() -> void {
        int tcp_read(0);
        char tcp_buffer[__self__::_buffer_max];
        memset(&tcp_buffer, 0, sizeof(tcp_buffer));
        if (__self__::IsConnected() && (tcp_read = read(*_sock, tcp_buffer, sizeof(tcp_buffer))) > 0) {
            tcp_request.raw_bytes = tcp_buffer;
            tcp_request.block_size = tcp_request.raw_bytes.length();
        }
    }).join();

    return tcp_request;
};

/**
 * @brief Reads TCP requests from the specified socket and stores the result in the provided TcpIntercept object.
 * 
 * @param _sock Pointer to the socket to read data from.
 * @param dest_obj Reference to the TcpIntercept object to store the result.
 */
void TcpInitializer::Socket::Read(t_sock *__restrict__ _sock, TcpInitializer::TcpIntercept &dest_obj) { 
    dest_obj = __self__::Read(_sock); 
};

/**
 * @brief Reads incoming TCP requests and returns the data as a string.
 * 
 * @returns A string containing the received socket data.
 */
const t_str TcpInitializer::Socket::Read2Str(void) { 
    return __self__::Read(__self__::_socket.get()).raw_bytes; 
};

/**
 * @brief Reads incoming TCP requests from the specified socket and returns the data as a string.
 * 
 * @param _sock Pointer to the socket used to read requests.
 * @returns A string containing the received socket data.
 */
const t_str TcpInitializer::Socket::Read2Str(t_sock *__restrict__ _sock) { 
    return __self__::Read(_sock).raw_bytes; 
};

/**
 * @brief Gets the pointer to the internal socket instance.
 * 
 * @returns A pointer to the internal socket instance.
 */
t_sock *TcpInitializer::Socket::GetSocket(void) noexcept {
    __self__::_AccessGuard();
    return __self__::_socket.get();
};

/**
 * @brief Closes the specified socket connection.
 * 
 * @param _sock Pointer to the socket to close.
 */
void TcpInitializer::Socket::Close(t_sock __restrict__ *_sock) noexcept {
    if (_sock != nullptr)
        close(*_sock);
};

/**
 * @brief Executes garbage collection for the socket.
 * 
 * This function cleans up resources and closes the socket.
 */
void TcpInitializer::Socket::GarbageCollectorExecute(void) noexcept {
    __self__::_AccessGuard();
    __local_enc.free();
    __self__::Close(__self__::_socket.get());
};

/**
 * @brief Sets the maximum number of socket connections allowed.
 * 
 * @param max The maximum number of connections.
 */
void TcpInitializer::Socket::SetMaxConnections(const t_u64 max) noexcept {
    if (max <= UINT64_MAX) {
        __self__::_accept_max = max;
    }
};

/**
 * @brief Checks if the socket can accept new TCP connections.
 * 
 * @returns true if the socket can accept new connections, false otherwise.
 */
bool TcpInitializer::Socket::CanAcceptTcp(void) noexcept { 
    return __self__::_tcp_count < __self__::_accept_max && __self__::SocketState(__self__::_socket.get()); 
};

/**
 * @brief Gets the current session count.
 * 
 * @returns A reference to the current session count.
 */
const t_u64 &TcpInitializer::Socket::GetSessionCount(void) noexcept { 
    return __self__::_tcp_count; 
};

/**
 * @brief Checks if the socket is currently connected.
 * 
 * @returns true if the socket is connected, false otherwise.
 */
bool TcpInitializer::Socket::IsConnected(void) noexcept {
    return __self__::_tcp_state == TcpState::CONNECTED && __self__::_tcp_state == TcpState::LISTENING && __self__::_socket.get() != nullptr && *__self__::_socket > 0 &&
           __self__::SocketState(__self__::_socket.get());
};

/**
 * @brief Checks the state of the specified socket.
 * 
 * @param _sock Pointer to the socket to check.
 * @returns true if the socket is in a valid state, false otherwise.
 */
bool TcpInitializer::Socket::SocketState(const t_sock *__restrict__ _sock) {
    if (_sock == nullptr || *_sock < 0)
        return false;
    int sock_state_val;
    socklen_t optlen(sizeof(sock_state_val));
    if (getsockopt(*_sock, SOL_SOCKET, SO_ERROR, &sock_state_val, &optlen) < 0) {
        return false;
    }
    return sock_state_val == 0;
};

/**
 * @brief Connects to a server at the specified address and port.
 * 
 * @tparam rT The type of the result to return (e.g., ClientTcpConnection or bool).
 * @param _address The remote address to connect to.
 * @param _port The port number to connect to.
 * @param _r Reference to the result variable.
 * @param _throw If true, exceptions will be thrown on errors.
 * @throws std::runtime_error If address validation fails or socket creation fails.
 */
template <typename rT> 
void TcpInitializer::Socket::_Connect(const t_strw _address, const t_u16 _port, rT _r, const bool _throw) {
    __self__::_AccessGuard();
    if (!__self__::_AddressValidate(_address, _port) && _throw) {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Conn Addr Eval failure"));
    }

    if (__self__::_socket == nullptr || *__self__::_socket <= 0) {
        __self__::_socket = std::make_unique<t_sock>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    }
    if (__self__::_socket.get() == nullptr && _throw) {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Sock open error"));
    }
    __self__::_tcp_state = TcpInitializer::TcpState::OPEN;
    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(struct sockaddr_in));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _address.data(), &(srv_addr.sin_addr)) < 1 && _throw) {
        throw std::runtime_error(__self__::_ErrorMsgCombine("address convert error"));
    }
    const bool status(connect(*__self__::_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == 0);
    if constexpr (std::is_same_v<rT, TcpInitializer::ClientTcpConnection &>) {
        _r.state = status;
        _r.sock = *__self__::_socket;
    } else if constexpr (std::is_same_v<rT, bool &>) {
        _r = status;
    }
    if (status) {
        __self__::_tcp_state = TcpInitializer::TcpState::CONNECTED;
    }
    return;
};

/**
 * @brief Accepts a new TCP connection on the specified socket.
 * 
 * @param _sock Pointer to the socket to accept connections from.
 * @param _sock_digest Pointer to store the new socket connection.
 * @returns true if a new connection was accepted, false otherwise.
 */
bool TcpInitializer::Socket::_Accept(t_sock *__restrict__ _sock, t_sock *__restrict__ _sock_digest) {
    if (__self__::IsConnected()) {
        if (__self__::SocketState(_sock)) {
            socklen_t addr_len(sizeof(*__self__::_sock_address));
            if (__self__::_tcp_count <= __self__::_accept_max) {
                ++__self__::_tcp_count;
                *_sock_digest = accept(*_sock, (struct sockaddr *)&(*__self__::_sock_address), &addr_len);
            }
        }
        return *_sock_digest > *_sock;
    }
    return false;
};

/**
 * @brief Binds the socket to the specified address and port.
 * 
 * @returns true if the binding was successful, false otherwise.
 */
bool TcpInitializer::Socket::_TcpBind(void) {
    __self__::_AccessGuard();
    if (__self__::SocketState(__self__::_socket.get())) {
        memset(__self__::_sock_address.get(), 0, sizeof(struct sockaddr_in));
        __self__::_sock_address->sin_port = htons(__self__::_port);
        __self__::_sock_address->sin_family = AF_INET;
        if (inet_pton(AF_INET, __self__::_ip_address.c_str(), &(__self__::_sock_address->sin_addr)) == 1) {
            return bind(*__self__::_socket, (struct sockaddr *)&(*__self__::_sock_address), sizeof(*__self__::_sock_address)) == 0;
        }
    }
    return false;
};

/**
 * @brief Listens for incoming TCP connections on the socket.
 * 
 * @returns true if the socket is successfully set to listen, false otherwise.
 */
bool TcpInitializer::Socket::_TcpListen(void) {
    __self__::_AccessGuard();
    if (__self__::SocketState(__self__::_socket.get())) {
        return listen(*__self__::_socket, __self__::_accept_max) == 0;
    }
    return false;
};

/**
 * @brief Validates the specified address and port.
 * 
 * @param _address The address to validate.
 * @param _port The port number to validate.
 * @returns true if the address and port are valid, false otherwise.
 */
bool TcpInitializer::Socket::_AddressValidate(const t_strw _address, const t_u16 _port) {
    if (_address.find(".") != ::__noindex) {
        if (std::count(_address.begin(), _address.end(), '.') == 3) {
            std::regex validator(TcpInitializer::IP_ADDRESS_REGX_ALLOW);
            return std::regex_match(_address.data(), validator);
        }
    }
    return false;
};

/**
 * @brief Handles exceptions by logging the error message.
 * 
 * @param error The error message to log.
 */
void TcpInitializer::Socket::_ExceptionHandle(const t_strw error) noexcept { 
    std::cerr << "Error: " << error.data() << "\n"; 
};

/**
 * @brief Combines an error message with the last error string.
 * 
 * @param _token The error token to combine.
 * @returns A string containing the combined error message.
 */
const t_str TcpInitializer::Socket::_ErrorMsgCombine(const t_strw _token) noexcept { 
    return t_str(_token.data(), _token.length()).append(strerror_l(errno, __local_enc.local_x)); 
};

/**
 * @brief Sets the verbosity of logging.
 * 
 * @param verbose If true, verbose logging is enabled.
 */
void TcpInitializer::Socket::SetVerbose(const bool verbose) noexcept { 
    __self__::verbose = verbose; 
};

/**
 * @brief Logs messages if verbose logging is enabled.
 * 
 * @tparam MT The types of the messages to log.
 * @param msgs The messages to log.
 */
template <typename... MT> 
void TcpInitializer::Socket::Log(MT... msgs) noexcept {
    if (TcpInitializer::Socket::verbose) {
        (std::cout << ... << msgs);
    }
};

/**
 * @brief Configures the socket to allow address reuse.
 * 
 * This function sets the SO_REUSEADDR and SO_REUSEPORT options on the socket.
 * @throws std::runtime_error If setting socket options fails.
 */
void TcpInitializer::Socket::_AddressReuse(void) {
    __self__::_AccessGuard();
    if (__self__::_socket != nullptr && *__self__::_socket > 0) {
        int opt_value(1);
        socklen_t opt_len(sizeof(opt_value));
        if (setsockopt(*__self__::_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, opt_len) < 0) {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Cannot set socket options!"));
        }
    }
};

/**
 * @brief Acquires a lock for thread safety.
 */
void TcpInitializer::Socket::_AccessGuard(void) noexcept { 
    std::unique_lock<decltype(__self__::_mtx)> Lock(__self__::_mtx); 
};

/**
 * @brief Destructor for the Socket class.
 * 
 * Cleans up resources and executes garbage collection.
 */
TcpInitializer::Socket::~Socket() { 
    TcpInitializer::Socket::GarbageCollectorExecute(); 
};

#endif
#endif
