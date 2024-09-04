#include "unix-g4tcpp_v0_0_1.cpp"

namespace TCP = TcpInitializer;
int main(int argc, char **argv)
{
    TCP::Socket::SetVerbose(true);
    TCP::Socket::SetMaxConnections(100);
    TCP::Socket::Init();
    TCP::Socket::CreateTcpServer("192.168.1.77", 3300);
    t_sock new_tcp;
    std::function<void(t_sock & _s)> exec_thread([&](t_sock &_s) -> void {
        std::thread([&] {
            int x(0);
            while (++x < 10)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
                TCP::Socket::Send(&_s, "New Message from Server");
            }
        }).detach();
    });

    std::function<void(t_sock& _s)> exec_tcp_read([&] (t_sock &_s) -> void {
        std::thread([&]() -> void {
            t_string tcp_read;
            while(!(tcp_read = TCP::Socket::Read2Str(&_s)).empty()){
                std::cout << "Received: " << tcp_read << "\n";
            }
        }).detach();
    });
    while ((new_tcp = TCP::Socket::AcceptTcpRequest()) > 0)
    {
        std::cout << "New TCP Connection!\n";

        exec_thread(new_tcp);

        exec_tcp_read(new_tcp);

    }
    return EXIT_SUCCESS;
}
