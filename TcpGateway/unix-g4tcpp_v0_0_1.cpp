#pragma once

#ifndef UNIX_TCP_INITIALIZER_VERSION
#include "unix-g4tcpp_v0_0_1.hpp"

using namespace ::TcpInitializer::_t; // types

using __self__ = ::TcpInitializer::Socket; // static reference object alias

const decltype(std::string::npos) __noindex = std::string::npos;




/**
 * Initializer socket tcp channel, required both for client and server.
 */
void TcpInitializer::Socket::Init(void) noexcept
{
    *__self__::_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
};





/**
 * Create Tcp Server Channel on _address and _port
 * @param __stringview remote address
 * @param __uint16 port number
 * @returns void
 */
void TcpInitializer::Socket::CreateTcpServer(const __stringview _address, const __uint16 _port)
{

    try
    {
        if (!__self__::_AddressValidate(_address, _port))
        {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Arg Eval failure"));
        }

        __self__::_ip_address = _address;
        __self__::_port = _port;

        if (__self__::_socket.get() == nullptr || *__self__::_socket <= 0)
        {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Socket failure"));
        }
        __self__::_tcp_state = TcpState::OPEN;

        __self__::Log("Tcp Channel: ", __self__::_ip_address, ":", __self__::_port, '\n');

        __self__::_AddressReuse();

        if (__self__::_TcpBind())
        {
            if (__self__::_TcpListen())
            {
                __self__::_tcp_state = TcpState::LISTENING;
            }
        }
    }
    catch (const __exception &e)
    {
        __self__::_ExceptionHandle(e.what());
    }
};




/**
 * Open Tcp socket connection on local address and _port
 * @param __uint16 port number
 * @returns void
 */
void TcpInitializer::Socket::CreateTcpServer(const __uint16 _port)
{
    __self__::CreateTcpServer(DEFAULT_IP_ADDRESS, _port);
};




/**
 * Client Side, connect to server on _address and _port
 * @param __stringview remote address
 * @param __uint16 port to connect
 * @returns void
 */
bool TcpInitializer::Socket::SyncConnect(const __stringview _address, const __uint16 _port)
{
    bool _r{false};
    std::function<void()> exec = 
    std::thread ().join();
    return _r;
};



/**
 * Client Side, connect to server on _address and _port
 * @param __stringview remote address
 * @param __uint16 port to connect
 * @returns void
 */
const TcpInitializer::ClientTcpConnection TcpInitializer::Socket::SyncConnect(const __stringview _address, const __uint16 _port, const bool _throw = false)
{
    TcpInitializer::ClientTcpConnection tcp_new;
    std::thread(__self__::__SyncConnect, _address, _port, _throw, tcp_new);
    return tcp_new;
};




__socket TcpInitializer::Socket::SyncAccept(__socket *__restrict__ _sock)
{
    thread_local __socket sock_digest;
    std::thread(__self__::__SyncAccept, _sock, &sock_digest).join();
    return sock_digest;
};




bool TcpInitializer::Socket::Send(const __stringview _buffer) noexcept
{
    char buffer[__self__::_buffer_max];

    memcpy(buffer, _buffer.data(), _buffer.length());
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        return send(*__self__::_socket, buffer, sizeof(buffer), 0) > 0;
    }
    return false;
};




bool TcpInitializer::Socket::Send(const __socket *__restrict__ _sock, const __stringview _buffer) noexcept
{
    if (_sock != nullptr && *_sock > 0)
    {
        return send(*_sock, _buffer.data(), _buffer.length(), 0) > 0;
    }
    return false;
};




TcpInitializer::TcpIntercept TcpInitializer::Socket::Read(__socket *__restrict__ _sock)
{
    TcpInitializer::TcpIntercept tcp_request;

    if (_sock != nullptr && *_sock > *__self__::_socket)
    {
        int tcp_read(0);
        char tcp_buffer[__self__::_buffer_max];

        while ((tcp_read = read(*_sock, tcp_buffer, sizeof(tcp_buffer))) > 0)
        {
            tcp_request.raw_bytes.assign(tcp_buffer);
            tcp_request.block_size = tcp_request.raw_bytes.length();
            tcp_request.payload_headers = "header-section";
        }
    }
    return tcp_request;
};




void TcpInitializer::Socket::Read(__socket *__restrict__ _sock, TcpInitializer::TcpIntercept &dest_obj)
{
    dest_obj = __self__::Read(_sock);
};




__socket *TcpInitializer::Socket::GetSocket(void) noexcept
{
    return __self__::_socket.get();
};




void TcpInitializer::Socket::Close(__socket __restrict__ *_sock) noexcept
{
    if (_sock != nullptr)
        close(*_sock);
};




void TcpInitializer::Socket::GarbageCollectorExecute(void) noexcept
{
    __local_enc.free();
    __self__::Close(__self__::_socket.get());
    __self__::_socket = nullptr;
};




void TcpInitializer::Socket::SetMaxConnections(const __uint64 max) noexcept
{
    if (max <= UINT64_MAX)
    {
        __self__::_accept_max = max;
    }
};




bool TcpInitializer::Socket::__SyncConnect(const __stringview& _address, const __uint16& _port, bool& _r)
{
    if (!__self__::_AddressValidate(_address, _port))
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Conn Addr Eval failure"));
    }
    if (__self__::_socket == nullptr || *__self__::_socket <= 0)
    {
        __self__::_socket = std::make_unique<__socket>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    }
    if (__self__::_socket.get() == nullptr)
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Sock open error"));
    }
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _address.data(), &(srv_addr.sin_addr)) < 1)
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("address convert error"));
    }
    std::cout << "Connection Socket " << *__self__::_socket << "\n";
    _r = (connect(*__self__::_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == 0);
    return _r;
};




void TcpInitializer::Socket::__SyncConnect(const __stringview _address, const __uint16 _port, const bool _throw = false, TcpInitializer::ClientTcpConnection& tcp_new)
{
    if (!__self__::_AddressValidate(_address, _port) && _throw)
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Conn Addr Eval failure"));
    }
    if (__self__::_socket == nullptr || *__self__::_socket <= 0)
    {
        __self__::_socket = std::make_unique<__socket>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    }
    if (__self__::_socket.get() == nullptr && _throw)
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("Sock open error"));
    }
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _address.data(), &(srv_addr.sin_addr)) < 1 && _throw)
    {
        throw std::runtime_error(__self__::_ErrorMsgCombine("address convert error"));
    }
    std::cout << "Connection Socket " << *__self__::_socket << "\n";
    tcp_new.state = connect(*__self__::_socket, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == 0;
    tcp_new.sock = *__self__::_socket;
    return;
};




bool TcpInitializer::Socket::__SyncAccept(__socket *__restrict__ _sock, __socket *__restrict__ _sock_digest)
{
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        socklen_t addr_len(sizeof(*__self__::_sock_address));
        if (__self__::_tcp_count <= __self__::_accept_max)
        {
            ++__self__::_tcp_count;
            *_sock_digest = accept(*_sock, (struct sockaddr *)&(*__self__::_sock_address), &addr_len);
        }
    }
    return *_sock_digest > *_sock;
};




bool TcpInitializer::Socket::_TcpBind(void)
{
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        __self__::_sock_address->sin_port = htons(__self__::_port);
        __self__::_sock_address->sin_family = AF_INET;
        if (inet_pton(AF_INET, __self__::_ip_address.c_str(), &(__self__::_sock_address->sin_addr)) == 1)
        {
            return bind(*__self__::_socket, (struct sockaddr *)&(*__self__::_sock_address), sizeof(*__self__::_sock_address)) == 0;
        }
    }
    return false;
};




bool TcpInitializer::Socket::_TcpListen(void)
{
    if (__self__::_socket != nullptr)
    {
        return listen(*__self__::_socket, __self__::_accept_max) == 0;
    }
    return false;
};




const bool TcpInitializer::Socket::_AddressValidate(const __stringview _address, const __uint16 _port)
{
    if (_address.find(".") != ::__noindex)
    {
        if (std::count(_address.begin(), _address.end(), '.') == 3)
        {
            std::regex validator(IP_ADDRESS_REGX_ALLOW);
            return std::regex_match(_address.data(), validator);
        }
    }
    return false;
};




void TcpInitializer::Socket::_ExceptionHandle(const __stringview error) noexcept
{
    std::cerr << "Error: " << error.data() << "\n";
};




const __string TcpInitializer::Socket::_ErrorMsgCombine(const __stringview _token) noexcept
{
    return __string(_token.data(), _token.length()).append(strerror_l(errno, __local_enc.local_x));
};




void TcpInitializer::Socket::SetVerbose(const bool verbose) noexcept
{
    __self__::verbose = verbose;
};





template <typename... MT> void TcpInitializer::Socket::Log(MT... msgs) noexcept
{
    if (TcpInitializer::Socket::verbose)
    {
        (std::cout << ... << msgs);
    }
};





void TcpInitializer::Socket::_AddressReuse(void)
{
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        int opt_value(1);
        socklen_t opt_len(sizeof(opt_value));
        if (setsockopt(*__self__::_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, opt_len) < 0)
        {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Cannot set socket options!"));
        }
    }
};





bool TcpInitializer::Socket::CanAcceptTcp(void) noexcept {
    return __self__::_tcp_count < __self__::_accept_max;
};





TcpInitializer::Socket::~Socket()
{
    TcpInitializer::Socket::GarbageCollectorExecute();
};

#endif
