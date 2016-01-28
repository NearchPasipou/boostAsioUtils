#ifndef BOOST_PROTO_SENDER_HEADER
#define BOOST_PROTO_SENDER_HEADER

#include <boost/asio.hpp>

namespace BoostProtoSender
{
    class ProtobufSender {
    public:
        inline ProtobufSender(boost::asio::ip::tcp::socket& socket);
    
        template <class MessageType, typename Callback> 
        void asyncSend(MessageType& msg, Callback cb);
    
    private:
        boost::asio::ip::tcp::socket    &_socket;
        boost::asio::streambuf          _streambuf;
        std::ostream                    _ostream;
    };
    
    template <class MessageType, typename Callback> 
    inline void ProtobufSender::asyncSend(MessageType& msg, Callback cb)
    {
        uint32_t size = htonl(msg.ByteSize());
        _ostream.write(reinterpret_cast<char*>(&size), sizeof(size));
        msg.SerializeToOstream(&_ostream);

        boost::asio::async_write(_socket, _streambuf, cb);
    }

    #include "ProtobufSender.inl"
}

#endif
