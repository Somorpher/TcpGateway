#include "unix-g4tcpp_v0_0_1.cpp"

using Gateway = TcpInitializer::Socket;

int main(int argc, char *argv[])
{

    __socket new_connection;

    Gateway::SetVerbose(true);

    Gateway::SetMaxConnections(4);

    Gateway::Init();

    Gateway::TcpServer("192.168.1.77", 3300);

    while ((new_connection = Gateway::NewRequest()) > 0)
    {
        Gateway::Log("Tcp Connection ", new_connection, "\n");

        if (!Gateway::CanAcceptTcp())
        {
            break;
        }

        const __string new_msg = Gateway::Read2Str(&new_connection); // read tcp requests from this socket instance

        std::cout << "Payload: " << new_msg << "\n";

        Gateway::Send(&new_connection, "message"); // payload dispacth block, endpoint data transmission on TCP
    }

    Gateway::GarbageCollectorExecute();



    return EXIT_SUCCESS;
}
