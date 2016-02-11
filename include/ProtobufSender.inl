
ProtobufSender::ProtobufSender(boost::asio::ip::tcp::socket& socket) :
    _socket(socket),
    _streambuf(),
    _ostream(&_streambuf)
{
}
