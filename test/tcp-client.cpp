#include "unix-g4tcpp_v0_0_1.cpp"

namespace TCP = TcpInitializer;

int main(int argc, char **argv)
{
    bool _e = true;
    TCP::Socket::SetVerbose(true);
    TCP::Socket::Init();
    TCP::Socket::Connect("127.0.0.1", 3300, true);

    std::function<void()> thread_execution_func([&]() -> void {
        t_string read_new_tcp_block;
        while(!(read_new_tcp_block = TCP::Socket::Read2Str()).empty()){
            std::cout << "Received: " << read_new_tcp_block << "\n";
        }
        std::cout << "Exited while loop!\n";
        _e = false;
    });

    std::thread(thread_execution_func).detach();

    int j(0);
    while(++j < 10){
        TCP::Socket::Send("message from client!");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    while(_e){
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }


    return EXIT_SUCCESS;
}
