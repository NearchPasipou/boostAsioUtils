#ifndef BOOST_CLIENT_HEADER
#define BOOST_CLIENT_HEADER

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>

#define DEFAULT_RECONNECT_TIME_IN_SECONDS 5

namespace BoostClient
{
    class Client {
    public:
        struct Config {
            inline Config(const std::string& address,
                          uint16_t port,
                          boost::posix_time::seconds interval,
                          std::size_t retries);

            boost::asio::ip::tcp::endpoint       _endPoint;
            boost::posix_time::seconds           _reconnectTime;
            std::size_t                          _numRetries;
        };

        inline Client(boost::asio::io_service& io, const Config& cfg);
        
        typedef boost::function<void(const boost::system::error_code&)> onClientConnectedCB_t;
    
        inline void initConnection(onClientConnectedCB_t onClientConnectedCB);
    
        inline boost::asio::ip::tcp::socket& getSocket();
   
    private:
        inline void onConnected(const boost::system::error_code& ec);
        inline void onReconnectTimer();
        inline void initReconnectTimer();
    
        onClientConnectedCB_t          _onClientConnectedCB;
        
        boost::asio::io_service        &_io;
        boost::asio::ip::tcp::socket   _clientSocket;
        boost::asio::deadline_timer    _reconnectTimer;
        Config                         _config;
    };
    
    #include "Client.inl"
}

#endif
