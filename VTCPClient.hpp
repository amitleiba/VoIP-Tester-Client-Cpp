#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Message.hpp"
#include "VTCPOpcode.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"

using  boost::asio::ip::tcp;

class VTCPClient
{
public:
    VTCPClient(std::shared_ptr<std::atomic<bool>> connected,
               std::function<void(const Message&)> onMessageReceived) :
        _socket(std::make_shared<tcp::socket>(_context)),
        _connected(std::move(connected)),
        _receiver(_socket, _connected,
                  std::bind(&VTCPClient::onDataReceived, this, std::placeholders::_1),
                  std::bind(&VTCPClient::disconnect, this)),
        _transmitter(_socket, _connected,
                     std::bind(&VTCPClient::disconnect, this)),
        _onMessageReceived(onMessageReceived)
    {

    }

    ~VTCPClient()
    {
        if(_thread.joinable())
        {
            _thread.join();
        }
    }

    void connect(const std::string& host, const std::string& port) {
        if (*_connected) {
            return;
        }

        tcp::resolver resolver(_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, port);
        boost::system::error_code ec;
        boost::asio::async_connect(*_socket, endpoints,
                             boost::bind(&VTCPClient::onConnect,
                                               this,
                                               boost::asio::placeholders::error));
        _thread = std::thread([this](){
            _context.run();
        });
    }

    void onConnect(const boost::system::error_code& ec)
    {
        if (ec)
        {
            onError(ec);
            return;
        }
        *_connected = true;
        _receiver.run();
//        *_connected = true;
        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_CONNECT_REQ));
        send(message);
    }

    void disconnect()
    {
        if (!(*_connected)) {
            return;
        }

        Message message;
        message.push(static_cast<int>(VTCPOpcode::VTCP_DISCONNECT_REQ));
        send(message);

        boost::system::error_code ec;
        _socket->close(ec);
        if(ec)
        {
            onError(ec);
        }
        _context.stop();
        _thread.join();
    }

    void send(const Message & message)
    {
        _transmitter.write(message.getAsBytes());
    }

    void onError(const boost::system::error_code &error)
    {
        std::cout << "*** The following exception has been thrown " << error.message() << " ***" << std::endl;
        disconnect();
    }

    void onDataReceived(std::vector<std::uint8_t> data)
    {
        _onMessageReceived(Message(data));
    }


private:
    boost::asio::io_context _context;
    std::shared_ptr<tcp::socket> _socket;
    std::shared_ptr<std::atomic<bool>> _connected;

    Receiver _receiver;
    Transmitter _transmitter;

    std::thread _thread;

    std::function<void(const Message &)> _onMessageReceived;
};
