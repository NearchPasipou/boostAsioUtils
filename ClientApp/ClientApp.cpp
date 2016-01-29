#include "ClientApp.hpp"
#include <iostream>
#include <arpa/inet.h>

ClientApp::ClientApp(boost::asio::io_service& io, const Client::Config& cfg) : 
    _client(io, cfg),
    _pbs(_client.getSocket()),
    _pbr(_client.getSocket())
{
    _statsRequest.set_sequence_id(0);
    _statsRequest.set_type(ClientAppProto::Request::QUERY_STATS);
}

void ClientApp::run()
{
    _client.initConnection(boost::bind(&ClientApp::onClientConnected, this, boost::asio::placeholders::error));
}

void ClientApp::handleConnectionFailure(const std::string& reason)
{
    std::cout << reason << std::endl;
    _client.getSocket().close();
}

void ClientApp::onClientConnected(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Failed to connect ") + ec.message());
        return;
    }

    std::cout << "Connected!!" << std::endl;

    _pbs.asyncSend(_statsRequest, boost::bind(&ClientApp::onRequestSent,
                                              this,
                                              boost::asio::placeholders::error));
}

void ClientApp::onRequestSent(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Failed to send request ") + ec.message());
        return;
    }

    _pbr.asyncReceive(_statsResponse, boost::bind(&ClientApp::onResponseReceived,
                                                  this,
                                                  boost::asio::placeholders::error));
}

void ClientApp::onResponseReceived(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Failed to receive response ") + ec.message());
        return;
    }

    std::cout <<_statsResponse.DebugString() << std::endl;
    std::cout << "stats received " << std::endl;

    _pbr.asyncReceive(_statsResponse, boost::bind(&ClientApp::onResponseReceived,
                                                  this,
                                                  boost::asio::placeholders::error));
}

