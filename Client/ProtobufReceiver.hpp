#ifndef BOOST_PROTO_RECEIVER_HEADER
#define BOOST_PROTO_RECEIVER_HEADER

#include <boost/asio.hpp>

namespace BoostProtoReceiver
{
    class ProtobufReceiver {
    public:
        inline ProtobufReceiver(boost::asio::ip::tcp::socket& socket);
    
        template <class MessageType, typename Callback> 
        void asyncReceive(MessageType& msg, Callback cb);
    
    private:
        template <class MessageType>
        void onHeaderReceived(const boost::system::error_code& ec, MessageType& msg);
        template <class MessageType>
        void onPayloadReceived(const boost::system::error_code& ec, MessageType& msg);

        boost::function<void(const boost::system::error_code&)> _cb;
        
        boost::asio::ip::tcp::socket    &_socket;
        uint32_t                        _responseSize;
        std::vector<uint8_t>            _readbuf;
    };
    
    template <class MessageType, class Callback>
    void ProtobufReceiver::asyncReceive(MessageType& msg, Callback cb)
    {
        _cb = cb;
        boost::asio::async_read(_socket,
                                boost::asio::buffer(&_responseSize, sizeof(_responseSize)),
                                boost::bind(&ProtobufReceiver::onHeaderReceived<MessageType>,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::ref(msg))
                                );
    }
    
    template <class MessageType>
    void ProtobufReceiver::onHeaderReceived(const boost::system::error_code& ec, MessageType& msg)
    {
        if (ec) {
            _socket.get_io_service().post(boost::bind(_cb, ec));
            return;
        }
    
        _responseSize = ntohl(_responseSize);
        _readbuf.resize(_responseSize);
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readbuf, _responseSize),
                                boost::bind(&ProtobufReceiver::onPayloadReceived<MessageType>, this, boost::asio::placeholders::error, boost::ref(msg)));
    }
    
    template <class MessageType>
    void ProtobufReceiver::onPayloadReceived(const boost::system::error_code& ec, MessageType& msg)
    {
        if (ec) {
            _socket.get_io_service().post(boost::bind(_cb, ec));
            return;
        }
    
        msg.ParseFromArray(&_readbuf[0], _readbuf.size());
        _socket.get_io_service().post(boost::bind(_cb, ec));
    }
    
    #include "ProtobufReceiver.inl"
}

#endif
