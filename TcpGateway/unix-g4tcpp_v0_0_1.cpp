#pragma once

#ifndef UNIX_TCP_INITIALIZER_VERSION
#include "unix-g4tcpp_v0_0_1.hpp"

using namespace ::TcpInitializer::_t; // types

using __self__ = ::TcpInitializer::Socket; // static reference object alias

const decltype(std::string::npos) __noindex = std::string::npos;

/**
 * Initializer socket tcp channel
 */
void TcpInitializer::Socket::Init(void) noexcept
{
    *__self__::_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
};

/**
 * Open Tcp socket connection with provided parameters
 * @param __stringview remote address
 * @param __uint16 port number for socket port binding
 * @returns void
 */
void TcpInitializer::Socket::Open(const __stringview _address, const __uint16 _port)
{

    try
    {
        if (!__self__::_AddressValidate(_address, _port))
        {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Parameter validation failure!"));
        }
        __self__::_ip_address = _address;
        __self__::_port = _port;

        *__self__::_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (__self__::_socket.get() == nullptr)
        {
            throw std::runtime_error(__self__::_ErrorMsgCombine("Socket creation failure!"));
        }
        __self__::_tcp_state = TcpState::OPEN;

        __self__::Log("Socket Opened on address ", __self__::_ip_address, ":", __self__::_port, '\n');

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

__socket TcpInitializer::Socket::Accept(__socket *__restrict__ _sock)
{
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        socklen_t addr_len(sizeof(*__self__::_sock_address));
        if (__self__::_tcp_count <= __self__::_accept_max)
        {
            ++__self__::_tcp_count;
            return accept(*__self__::_socket, (struct sockaddr *)&(*__self__::_sock_address), &addr_len);
        }
    }
    return -1;
};

bool TcpInitializer::Socket::Send(const __stringview _buffer) noexcept
{
    if (__self__::_socket != nullptr && *__self__::_socket > 0)
    {
        return send(*__self__::_socket, _buffer.data(), _buffer.length(), 0) > 0;
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

TcpInitializer::TcpPayloadRequest TcpInitializer::Socket::Receive(__socket *__restrict__ _sock)
{
    TcpInitializer::TcpPayloadRequest tcp_request;
    if (_sock != nullptr && *_sock > *__self__::_socket)
    {
        char tcp_buffer[__self__::_buffer_max];
        int tcp_read(0);
        if((tcp_read = read(*_sock, tcp_buffer, sizeof(tcp_buffer))) > 0){
            tcp_request.raw_bytes.assign(tcp_buffer);
            tcp_request.block_size = tcp_request.raw_bytes.length();
        }
    }
    return tcp_request;
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

/**
 * verify parameters, port number must be <= 65535 and address a valid ip address, return true if success
 * @param __stringview address
 * @param __uint16 port number(0-65535)
 * @returns bool true on success, false on failure
 */
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

/**
 * Graceful exception handling
 * @param __stringview error message string
 * @returns void
 */
void TcpInitializer::Socket::_ExceptionHandle(const __stringview error) noexcept
{
    std::cerr << "Error: " << error.data() << "\n";
};

TcpInitializer::Socket::~Socket()
{
    TcpInitializer::Socket::GarbageCollectorExecute();
};

const __string TcpInitializer::Socket::_ErrorMsgCombine(const __stringview _token) noexcept
{
    return __string(_token.data(), _token.length()).append(strerror_l(errno, __local_enc.local_x));
};

void TcpInitializer::Socket::SetVerbose(const bool verbose) noexcept
{
    __self__::verbose = verbose;
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
        if (setsockopt(*__self__::_socket.get(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_value, opt_len) >= 0)
        {
            return;
        }
    }
    throw std::runtime_error(__self__::_ErrorMsgCombine("Cannot set socket options!"));
};

#endif
