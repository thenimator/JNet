
#include <string_view>
#include <iostream>
#include "Client.hpp"



using namespace JNet::udp;



Client::Client(std::string_view host) {
    using namespace boost::asio;
    ip::udp::resolver resolver(context);
    try {
        std::string test = host.data();
        endpoint = *resolver.resolve(ip::udp::v4(),host,"16632").begin();
        
    } catch (boost::system::system_error& e) {
        std::cout << "Error when connecting with " << host << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Code: " << e.code() << std::endl;
    }
}

Client::~Client() {
    if (!shouldDisconnect) {
        std::cerr << "You should call disconnect() before deleting the client" << std::endl;
        disconnect();
    }
    receiver.join();
}

void Client::connect() {
    activeConnection = true;
    shouldDisconnect = false;
    receiver = std::thread(boost::bind(&Client::receiveData, this));
    
    
}

void Client::disconnect() {
    std::cerr << "disconnecting" << std::endl;
    shouldDisconnect = true;
    
}

bool Client::hasConnection() {
    
    return activeConnection;
    
}

void Client::receiveData() {
    using namespace boost::asio;
    try {
        ip::udp::socket socket(context);
        socket.open(ip::udp::v4());
        while (!shouldDisconnect) {
            Header header = {messageCount};
            messageCount++;
            std::array<bool,sizeof(Header)> data;
            //std::copy_n(&header, data.size(), data.begin());
            memcpy(data.data(),&header,data.size());
            socket.send_to(boost::asio::buffer(data),endpoint);
            std::cout << "Message sent to " << endpoint.address() << "\n";
            std::array<uint8_t, sizeof(Message)> receivedData;
            Message message;
            size_t size = socket.receive_from(boost::asio::buffer(receivedData),endpoint);
            std::cout << "Got response\n";
            *(std::array<uint8_t, sizeof(Message)> *)&message = receivedData;

            messages.push(message);
            //std::string receivedMessage((const char* )&message.data,message.header.messageLength);
            //std::cout << message.header.id << "\n" << receivedMessage << "\n\n"; 
        }
        
    
    } catch (boost::system::system_error& e) {
        std::cerr << e.what() << std::endl;
    }
    activeConnection = false;
}






