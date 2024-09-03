#include "unix-g4tcpp_v0_0_1.cpp"

using TcpClient = TcpInitializer::Socket;

int main(int argc, char **argv)
{
    TcpClient::SetVerbose(true); // log messages during operation execution

    TcpClient::Init(); // initialize socket pointer and other data

    if (TcpClient::Connect("1127.0.0.1", 3300, true).state) // connect sync mode
    {        

        TcpClient::Send("message"); // send message

        __string new_msg = TcpClient::Read2Str();

        std::cout << "New Message: " << new_msg << "\n";
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return EXIT_SUCCESS;
};
