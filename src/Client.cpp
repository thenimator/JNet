#include "Client.hpp"

Client::Client(const std::string& host) {
    using namespace boost::asio::ip;
    udp::resolver resolver(context);
    try {
        std::string test = host;
        endpoint = *resolver.resolve(udp::v4(),host,"localhost").begin();
        
    } catch (boost::system::system_error& e) {
        std::cout << "Error: " << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Code: " << e.code() << std::endl;
    }
}

Client::~Client() {
    
}

void Client::connect() {
    
    receiver = std::thread(boost::bind(&Client::receiveData, this));
    
}

void Client::receiveData() {
    using namespace boost::asio::ip;
    try {
        udp::socket socket(context);
        socket.open(udp::v4());
        activeConnection = true;
        while (!shouldDisconnect) {
            Header header = {messageCount};
            messageCount++;
            std::array<bool,sizeof(Header)> data;
            //std::copy_n(&header, data.size(), data.begin());
            memcpy(data.begin(),&header,data.size());
            socket.send_to(boost::asio::buffer(data),endpoint);
            std::array<uint8_t, sizeof(Message)> receivedData;
            Message message;
            size_t size = socket.receive_from(boost::asio::buffer(receivedData),endpoint);
            *(std::array<uint8_t, sizeof(Message)> *)&message = receivedData;

            messages.push(message);
        }
        
    
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    activeConnection = false;
}

