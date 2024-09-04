/**
 *
 * Unix Gateway for Tcp Protocol v0.0.1
 *
 */

#pragma once

// check os and architecture
#if defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__unix) || defined(__ANDROID__) || defined(__FreeBSD__)

// architecture detections
#if defined(__x86_64__) || defined(__MX64) || defined(__aarch64__) || defined(__powerpc__) || defined(__sparc__) || defined(__mips__) || defined(__riscv)

// library inclusion
#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <err.h>
#include <errno.h>
#include <error.h>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <locale.h>
#include <memory>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <mutex>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>

#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// global version macro identifies library version
#ifndef UNIX_TCP_INITIALIZER_VERSION
#define UNIX_TCP_INITIALIZER_VERSION (const char[6]) "0.0.1"

namespace TcpInitializer
{

// protect from namespace polution
namespace _t
{

using t_stringw = std::basic_string_view<char>;
using t_string = std::basic_string<char>;
using t_u16 = std::uint16_t;
using t_u32 = std::uint32_t;
using t_u64 = std::uint64_t;
using t_except = std::exception;
using t_sock = int;

}; // namespace _t

#define IP_ADDRESS_REGX_ALLOW t_string("^[0-9.]+$")
#define DEFAULT_IP_ADDRESS t_string("127.0.0.1")
#define DEFAULT_PORT_NUMBER 3300u
#define DEFAULT_ACCEPT_MAX 100u
#define DEFAULT_BUFFER_MAX_SIZE 4096u

using namespace _t; // use internally

enum class TcpState
{
    NONE,
    OPEN,
    LISTENING,
    CONNECTED,
    FAILED
};

enum class TcpConnectionType {
  PERSISTENT = 0,
  STATELESS
};

typedef struct alignas(void *)
{
    locale_t local_x{newlocale(LC_ALL_MASK, "en_US.UTF-8", (locale_t)0)};
    bool freed{};
    void free() noexcept
    {
        if (!freed)
            freelocale(local_x);
    };
} local_encoding;

typedef struct alignas(void *)
{
    t_string raw_bytes{};
    t_u64 block_size{};
} TcpIntercept;

typedef struct alignas(void *)
{
    t_sock sock{};
    bool state{};
} ClientTcpConnection;

local_encoding __local_enc;

class Socket
{
  protected: // protected member variables
    static std::unique_ptr<t_sock> _socket;
    static std::unique_ptr<struct sockaddr_in> _sock_address;
    static TcpState _tcp_state;
    static t_string _ip_address;
    static t_u16 _port;
    static t_u64 _accept_max;
    static t_u64 _tcp_count;
    static t_u64 _buffer_max;
    static std::mutex _mtx;
    

  public: // public member variables
    static bool verbose;
    using __self__ = TcpInitializer::Socket;

  public: // public member functions
    Socket() = delete;

    __attribute__((cold)) inline static void Init(void) noexcept;

    __attribute__((cold)) static void CreateTcpServer(const t_u16 _port);

    __attribute__((cold)) static void CreateTcpServer(const t_stringw _address, const t_u16 _port);

    __attribute__((cold)) inline static bool Connect(const t_stringw _address, const t_u16 _port);

    __attribute__((cold)) inline static const ClientTcpConnection Connect(const t_stringw _address, const t_u16 _port, const bool _throw);

    __attribute__((hot, access(read_only, 1))) inline static t_sock AcceptTcpRequest(t_sock *__restrict__ _sock);

    __attribute__((hot)) inline static t_sock AcceptTcpRequest(void);

    __attribute__((hot)) inline static bool Send(const t_stringw _buffer) noexcept;

    __attribute__((hot, access(read_only, 1))) inline static bool Send(const t_sock *__restrict__ _sock, const t_stringw _buffer) noexcept;

    __attribute__((hot, warn_unused_result, warn_unused_result)) inline static TcpIntercept Read(void);

    __attribute__((hot, warn_unused_result, warn_unused_result)) inline static const t_string Read2Str(void);

    __attribute__((hot, warn_unused_result, warn_unused_result)) inline static const t_string Read2Str(t_sock *__restrict__ _sock);

    __attribute__((hot)) inline static void Read(t_string& sink_frame);

    __attribute__((hot, access(read_only, 1))) inline static void Read(t_sock *__restrict__ _sock, t_string& sink_frame);

    __attribute__((hot, warn_unused_result, access(read_only, 1), warn_unused_result)) inline static TcpIntercept Read(t_sock *__restrict__ _sock);

    __attribute__((hot, access(read_only, 1))) inline static void Read(t_sock *__restrict__ _sock, TcpIntercept &dest_obj);

    __attribute__((const, warn_unused_result)) inline static t_sock *GetSocket(void) noexcept;

    __attribute__((cold)) inline static void Close(t_sock __restrict__ *_sock) noexcept;

    __attribute__((cold, zero_call_used_regs("all"))) inline static void GarbageCollectorExecute(void) noexcept;

    __attribute__((cold)) inline static void SetVerbose(const bool verbose) noexcept;

    __attribute__((cold)) inline static void SetMaxConnections(const t_u64 max) noexcept;

    template <typename... MT> __attribute__((hot)) inline static void Log(MT... msgs) noexcept;

    __attribute__((hot)) inline static bool CanAcceptTcp(void) noexcept;

    __attribute__((cold, const, warn_unused_result)) inline static const decltype(TcpInitializer::Socket::_tcp_count) &GetSessionCount(void) noexcept;

    __attribute__((cold, warn_unused_result)) inline static const bool IsConnected(void) noexcept;

    __attribute__((cold, warn_unused_result, access(read_only, 1))) inline static bool SocketState(const t_sock *__restrict__ _sock);

    
    ~Socket();

  protected: // protected member functions
    
    template <typename rT> __attribute__((cold)) inline static void _Connect(const t_stringw _address, const t_u16 _port, rT _r, const bool _throw = false);

    __attribute__((hot, access(read_only, 1))) inline static bool _Accept(t_sock *__restrict__ _sock, t_sock *__restrict__ _sock_digest);

    __attribute__((cold, warn_unused_result, pure, nothrow)) static const bool _AddressValidate(const t_stringw _address, const t_u16 _port);

    __attribute__((cold, nothrow)) static void _ExceptionHandle(const t_stringw error) noexcept;

    __attribute__((hot, const, warn_unused_result)) static const t_string _ErrorMsgCombine(const t_stringw _token) noexcept;

    __attribute__((cold, )) inline static bool _TcpBind(void);

    __attribute__((cold, )) inline static bool _TcpListen(void);

    __attribute__((cold)) inline static void _AddressReuse(void);

    __attribute__((hot)) inline static void _AccessGuard(void) noexcept;
};
}; // namespace TcpInitializer

#endif

#endif

#endif

std::unique_ptr<TcpInitializer::t_sock> TcpInitializer::Socket::_socket = std::make_unique<TcpInitializer::t_sock>(-1);

std::unique_ptr<struct sockaddr_in> TcpInitializer::Socket::_sock_address = std::make_unique<struct sockaddr_in>();

TcpInitializer::t_string TcpInitializer::Socket::_ip_address = TcpInitializer::t_string(DEFAULT_IP_ADDRESS);

TcpInitializer::t_u16 TcpInitializer::Socket::_port = DEFAULT_PORT_NUMBER;

bool TcpInitializer::Socket::verbose = false;

TcpInitializer::t_u64 TcpInitializer::Socket::_accept_max = DEFAULT_ACCEPT_MAX;

TcpInitializer::t_u64 TcpInitializer::Socket::_tcp_count = 0;

TcpInitializer::t_u64 TcpInitializer::Socket::_buffer_max = DEFAULT_BUFFER_MAX_SIZE;

TcpInitializer::TcpState TcpInitializer::Socket::_tcp_state = TcpInitializer::TcpState::NONE;

std::mutex TcpInitializer::Socket::_mtx = std::mutex();
