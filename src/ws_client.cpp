#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <functional>
#include <memory>
#include <thread>

#include "base64.hpp"
#include "ws_client.hpp"
#include <stdexcept>



namespace astro {

using json = nlohmann::json;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


WSClient::WSClient(const std::string& url) : url_(url) {
    client_.clear_access_channels(websocketpp::log::alevel::all);
    client_.init_asio();

    client_.set_message_handler(bind(&WSClient::on_message, this, _1, _2));
    client_.set_open_handler(bind(&WSClient::on_open, this, _1));
    client_.set_close_handler(bind(&WSClient::on_close, this, _1));
}

void WSClient::connect(TextMessageCallback text_callback, AudioMessageCallback audio_callback) {
    text_callback_ = text_callback;
    audio_callback_ = audio_callback;

    websocketpp::lib::error_code ec;
    client_.set_tls_init_handler(bind(&WSClient::on_tls_init, this, _1));

    connection_ = client_.get_connection(url_, ec);
    if (ec) {
        throw std::runtime_error("Could not create connection: " + ec.message());
    }

    client_.connect(connection_);
    client_thread_ = std::thread([this]() {
        client_.run();
    });
}

websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>  WSClient::on_tls_init(websocketpp::connection_hdl hdl) {
    // Create a new context
    auto ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

    // Set the CA file path to the system CA file
    ctx->set_options(asio::ssl::context::default_workarounds |
                     asio::ssl::context::no_sslv2 |
                     asio::ssl::context::single_dh_use);

    // Set the CA file path to the system CA file
    ctx->set_default_verify_paths();    

    return ctx;
}

bool WSClient::is_connected() const {
    return connected_;
}

void WSClient::send_message(const std::string& script) {
    if (!connection_) {
        throw std::runtime_error("Not connected");
    }

    json message = {
        {"script", script}
    };

    websocketpp::lib::error_code ec;
    client_.send(connection_->get_handle(), message.dump(), websocketpp::frame::opcode::text, ec);
    printf("Sent message: %s\n", message.dump().c_str());
    if (ec) {
        throw std::runtime_error("Failed to send message: " + ec.message());
    }
}

void WSClient::disconnect() {
    printf("Disconnecting\n");
    if (connected_ && connection_) {
        connection_->close(websocketpp::close::status::normal, "Closing connection");
    }
    if (client_thread_.joinable()) {
        client_thread_.join();
    }
}

WSClient::~WSClient() {
    disconnect();
}

void WSClient::on_message(websocketpp::connection_hdl hdl, Client::message_ptr msg) {
    try {
        auto j = json::parse(msg->get_payload());
        printf("Received message: %s\n", msg->get_payload().c_str());

        std::string type = j["type"];
        std::vector<uint8_t> audio;
        if (type == "text") {
            std::string content = j["content"];
            if (!content.empty()) {
                text_callback_(content);
            }
        } else if (type == "audio") {
            std::string audio_base64 = j["chunk"];
            // Convert base64 to binary
            audio = base64_decode(audio_base64);
            //audio_callback_(audio);
        }
    } catch (const std::exception& e) {
        printf("Error parsing message: %s\n", e.what());
    }
}

void WSClient::on_open(websocketpp::connection_hdl hdl) {
    printf("Connection established\n");// Connection established
    connected_ = true;
}

void WSClient::on_close(websocketpp::connection_hdl hdl) {
    printf("Connection closed\n");// Connection closed
    connected_ = false;
}

} // namespace astro
