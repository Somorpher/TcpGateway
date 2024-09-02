/**
 *
 * Unix Gateway for Tcp Protocol v0.0.1
 *
 */

#pragma once

// check os and architecture
#if defined(__linux__) || defined(__gnu_linux__) || defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__unix) ||               \
    defined(__ANDROID__) || defined(__FreeBSD__)

// architecture detections
#if defined(__x86_64__) || defined(__MX64) || defined(__aarch64__) || defined(__powerpc__) || defined(__sparc__) || defined(__mips__) ||             \
    defined(__riscv)

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
#include <string_view>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>

#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>

// global version macro identifies library version
#ifndef UNIX_TCP_INITIALIZER_VERSION
#define UNIX_TCP_INITIALIZER_VERSION (const char[6]) "0.0.1"

namespace TcpInitializer
{

// protect from namespace polution
namespace _t
{

using __stringview = std::basic_string_view<char>;
using __string = std::basic_string<char>;

using __uint16 = std::uint16_t;
using __uint32 = std::uint32_t;
using __uint64 = std::uint64_t;
using __exception = std::exception;

using __socket = int;

#define IP_ADDRESS_REGX_ALLOW __string("^[0-9.]+$")
#define DEFAULT_IP_ADDRESS __string("127.0.0.1")
#define DEFAULT_PORT_NUMBER 3300u
#define DEFAULT_ACCEPT_MAX 100u
#define DEFAULT_BUFFER_MAX_SIZE 4096u

}; // namespace _t

using namespace _t; // use internally

enum class TcpState
{
    NONE,
    OPEN,
    LISTENING,
    CONNECTED,
    FAILED
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
    __string payload_headers{};
    __string raw_bytes{};
    __uint64 block_size{};
} TcpIntercept;

typedef struct alignas(void*) {
  __socket sock{};
  bool state{};
} ClientTcpConnection;

local_encoding __local_enc;

class Socket
{
  protected: // protected member variables
    static std::unique_ptr<__socket> _socket;
    static std::unique_ptr<struct sockaddr_in> _sock_address;
    static __string _ip_address;
    static __uint16 _port;
    static __uint64 _accept_max;
    static __uint64 _tcp_count;
    static __uint64 _buffer_max;
    static TcpState _tcp_state;

  public: // public member variables
    static bool verbose;
    using __self__ = TcpInitializer::Socket;

  public: // public member functions
    Socket() = delete;

    __attribute__((cold)) inline static void Init(void) noexcept;

    __attribute__((cold)) static void CreateTcpServer(const __uint16 _port);

    __attribute__((cold)) static void CreateTcpServer(const __stringview _address, const __uint16 _port);

    __attribute__((cold)) inline static bool SyncConnect(const __stringview _address, const __uint16 _port);

    __attribute__((cold)) inline static const ClientTcpConnection SyncConnect(const __stringview _address, const __uint16 _port, const bool _throw);

    __attribute__((hot, access(read_only, 1))) inline static __socket SyncAccept(__socket *__restrict__ _sock);

    __attribute__((hot)) inline static bool Send(const __stringview _buffer) noexcept;

    __attribute__((hot, access(read_only, 1))) inline static bool Send(const __socket *__restrict__ _sock, const __stringview _buffer) noexcept;

    __attribute__((hot, warn_unused_result, access(read_only, 1), warn_unused_result)) inline static TcpIntercept Read(__socket *__restrict__ _sock);

    __attribute__((hot, access(read_only, 1))) inline static void Read(__socket *__restrict__ _sock, TcpIntercept& dest_obj);

    __attribute__((const, warn_unused_result)) inline static __socket *GetSocket(void) noexcept;

    __attribute__((cold)) inline static void Close(__socket __restrict__ *_sock) noexcept;

    __attribute__((cold, zero_call_used_regs("all"))) inline static void GarbageCollectorExecute(void) noexcept;

    __attribute__((cold)) inline static void SetVerbose(const bool verbose) noexcept;

    __attribute__((cold)) inline static void SetMaxConnections(const __uint64 max) noexcept;

    template <typename... MT> __attribute__((hot)) inline static void Log(MT... msgs) noexcept;

    __attribute__((hot)) inline static bool CanAcceptTcp(void) noexcept;

    ~Socket();

  protected: // protected member functions

    __attribute__((cold)) inline static bool __SyncConnect(const __stringview &_address, const __uint16& _port, bool& _r);

    __attribute__((cold)) inline static void __SyncConnect(const __stringview _address, const __uint16 _port, const bool _throw, TcpInitializer::ClientTcpConnection& _r);

    __attribute__((hot, access(read_only, 1))) inline static bool __SyncAccept(__socket *__restrict__ _sock, __socket *__restrict__ _sock_digest);

    __attribute__((cold, warn_unused_result, pure, nothrow)) static const bool _AddressValidate(const __stringview _address, const __uint16 _port);

    __attribute__((cold, nothrow)) static void _ExceptionHandle(const __stringview error) noexcept;

    __attribute__((hot, const, warn_unused_result)) static const __string _ErrorMsgCombine(const __stringview _token) noexcept;

    __attribute__((cold, )) inline static bool _TcpBind(void);

    __attribute__((cold, )) inline static bool _TcpListen(void);

    __attribute__((cold)) inline static void _AddressReuse(void);
};
}; // namespace TcpInitializer

#endif

#endif

#endif

std::unique_ptr<TcpInitializer::__socket> TcpInitializer::Socket::_socket = std::make_unique<TcpInitializer::__socket>(-1);

std::unique_ptr<struct sockaddr_in> TcpInitializer::Socket::_sock_address = std::make_unique<struct sockaddr_in>();

TcpInitializer::__string TcpInitializer::Socket::_ip_address = TcpInitializer::__string(DEFAULT_IP_ADDRESS);

TcpInitializer::__uint16 TcpInitializer::Socket::_port = DEFAULT_PORT_NUMBER;

bool TcpInitializer::Socket::verbose = false;

TcpInitializer::__uint64 TcpInitializer::Socket::_accept_max = DEFAULT_ACCEPT_MAX;

TcpInitializer::__uint64 TcpInitializer::Socket::_tcp_count = 0;

TcpInitializer::__uint64 TcpInitializer::Socket::_buffer_max = DEFAULT_BUFFER_MAX_SIZE;

TcpInitializer::TcpState TcpInitializer::Socket::_tcp_state = TcpInitializer::TcpState::NONE;
