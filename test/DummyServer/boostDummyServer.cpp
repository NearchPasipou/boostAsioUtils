#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "ProtobufSender.hpp"
#include "ProtobufReceiver.hpp"
#include "ProtoMsgRequest.pb.h"
#include "ProtoMsgResponse.pb.h"

class DummyServer {
public:
    DummyServer(boost::asio::io_service& io);

    void initAccepting();
    void closeAcceptor();

private:
    boost::asio::io_service        &_io;
    boost::asio::ip::tcp::socket   _socket;
    boost::asio::deadline_timer    _resendTimer;
    boost::asio::ip::tcp::endpoint _ep;
    boost::asio::ip::tcp::acceptor _acceptor;
    
    BoostProtoSender::ProtobufSender     _pbs;
    BoostProtoReceiver::ProtobufReceiver _pbr;

    ProtoMsg::Request  _statsRequest;
    ProtoMsg::Response _statsResponse;

    void onResponseSent(const boost::system::error_code& ec);
    void onRequestReceived(const boost::system::error_code& ec);
    void onClientConnected(const boost::system::error_code& ec);
    void handleConnectionFailure(const std::string& reason);
    void onResendTimer();
    void initResendTimer();
};

DummyServer::DummyServer(boost::asio::io_service& io) :
    _io(io),
    _socket(_io),
    _resendTimer(_io),
    _ep(boost::asio::ip::address::from_string("127.0.0.1"), 40000),
    _acceptor(_io, _ep),
    _pbs(_socket),
    _pbr(_socket)
   
{
    _statsResponse.set_sequence_id(0);
    _statsResponse.set_type(ProtoMsg::Response::STATISTICS);
    ProtoMsg::Response_Stats *msg_stats = _statsResponse.mutable_statistics();
    msg_stats->set_total(10);
    msg_stats->set_up(8);
}

void DummyServer::handleConnectionFailure(const std::string& reason)
{
    std::cout << reason << std::endl;
    closeAcceptor();
}

void DummyServer::initAccepting()
{
    _acceptor.async_accept(_socket, boost::bind(&DummyServer::onClientConnected, this, boost::asio::placeholders::error));
}

void DummyServer::closeAcceptor()
{
    _acceptor.close();
}

void DummyServer::onResponseSent(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Failed to send response ") + ec.message());
        return;
    }

    std::cout << "Response sent!!" << std::endl;
    initResendTimer();
}

void DummyServer::onRequestReceived(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Failed to receive request: ") + ec.message());
        return;
    }
    
    std::cout <<_statsRequest.DebugString() << std::endl;
    std::cout << "stats request received " << std::endl;

    _pbs.asyncSend(_statsResponse, boost::bind(&DummyServer::onResponseSent,
                                              this,
                                              boost::asio::placeholders::error));
}

void DummyServer::onClientConnected(const boost::system::error_code& ec)
{
    if (ec) {
        handleConnectionFailure(std::string("Client failed to connect: ") + ec.message());
        return;
    }

    std::cout << "Someone connected!!" << std::endl;

    _pbr.asyncReceive(_statsRequest, boost::bind(&DummyServer::onRequestReceived,
                                                 this,
                                                 boost::asio::placeholders::error));
}

void DummyServer::onResendTimer()
{
    std::cout << "OnResendTimer!!" << std::endl;
    _pbs.asyncSend(_statsResponse, boost::bind(&DummyServer::onResponseSent,
                                               this,
                                               boost::asio::placeholders::error));
}

void DummyServer::initResendTimer()
{
    std::cout << "InitResendTimer!!" << std::endl;
    _resendTimer.expires_from_now(boost::posix_time::seconds(5));
    _resendTimer.async_wait(boost::bind(&DummyServer::onResendTimer, this));
}

int main()
{
    boost::asio::io_service io;
    DummyServer DummyServer(io);
    DummyServer.initAccepting();
    io.run();

    DummyServer.closeAcceptor();
}

