#pragma once

#include <string>
#include <vector>
#include <functional>
#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace astro {

class WSClient {
public:
    using AudioMessageCallback = std::function<void(const std::vector<uint8_t>& audio)>;
    using TextMessageCallback = std::function<void(const std::string& text)>;

    WSClient(const std::string& url);
    ~WSClient();

    void connect(TextMessageCallback text_callback, AudioMessageCallback audio_callback);
    void send_message(const std::string& script);
    void disconnect();
    
private:
    using Client = websocketpp::client<websocketpp::config::asio_tls_client>;
    using ConnectionPtr = Client::connection_ptr;

    void on_message(websocketpp::connection_hdl hdl, Client::message_ptr msg);
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);

    websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> on_tls_init(websocketpp::connection_hdl hdl);

    Client client_;
    ConnectionPtr connection_;
    std::string url_;
    TextMessageCallback text_callback_;
    AudioMessageCallback audio_callback_;
    std::thread client_thread_;
};

} // namespace astro
