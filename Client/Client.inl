
Client::Config::Config(const std::string& address, uint16_t port, boost::posix_time::seconds interval, std::size_t retries) :
    _endPoint(boost::asio::ip::address::from_string(address), port),
    _reconnectTime(interval),
    _numRetries(retries)
{
}

Client::Client(boost::asio::io_service& io, const Config& cfg) :
    _io(io),
    _clientSocket(io),
    _reconnectTimer(io),
    _config(cfg)
{
}

void Client::initConnection(onClientConnectedCB_t onClientConnectedCB)
{
    _onClientConnectedCB = onClientConnectedCB;
    _clientSocket.async_connect(_config._endPoint, boost::bind(&Client::onConnected, this, boost::asio::placeholders::error));
}

void Client::onConnected(const boost::system::error_code& ec)
{
    if (ec) {
        if (_config._numRetries--) {
            initReconnectTimer();
        }
    }
    else {
        _io.post(boost::bind(_onClientConnectedCB, ec));
    }
}

void Client::onReconnectTimer()
{
    _clientSocket.async_connect(_config._endPoint, boost::bind(&Client::onConnected, this, boost::asio::placeholders::error));
}

void Client::initReconnectTimer()
{
    _reconnectTimer.expires_from_now(boost::posix_time::seconds(_config._reconnectTime));
    _reconnectTimer.async_wait(boost::bind(&Client::onReconnectTimer, this));
}

boost::asio::ip::tcp::socket &Client::getSocket()
{
    return _clientSocket;
}

