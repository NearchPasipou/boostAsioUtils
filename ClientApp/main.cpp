#include "ClientApp.hpp"
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 67677
#define RECONNECT_TIME 1
#define RECONNECT_RETRIES 3

int main()
{
    BoostClient::Client::Config cfg(SERVER_ADDRESS, SERVER_PORT, boost::posix_time::seconds(RECONNECT_TIME), (std::size_t)RECONNECT_RETRIES);
    boost::asio::io_service io;
    ClientApp ClientApp(io, cfg);
    ClientApp.run();
    io.run();
}

