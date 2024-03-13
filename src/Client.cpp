#include "Client.hpp"

Client::Client(const std::string& host) {
    using namespace boost::asio::ip;
    udp::resolver resolver(context);
    try {
        std::string test = host;
        endpoint = *resolver.resolve(udp::v4(),host,"16632").begin();
        
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
    using namespace boost::asio::ip;
    try {
        udp::socket socket(context);
        socket.open(udp::v4());
        while (!shouldDisconnect) {
            std::cout << "While" << "\n";
            Header header = {messageCount};
            messageCount++;
            std::array<bool,sizeof(Header)> data;
            //std::copy_n(&header, data.size(), data.begin());
            memcpy(data.begin(),&header,data.size());
            socket.send_to(boost::asio::buffer(data),endpoint);
            std::array<uint8_t, sizeof(Message)> receivedData;
            Message message;
            std::cout << "About to receive\n";
            size_t size = socket.receive_from(boost::asio::buffer(receivedData),endpoint);
            std::cout << "Received\n";
            *(std::array<uint8_t, sizeof(Message)> *)&message = receivedData;

            messages.push(message);
            std::cout << message.id << "\n";
        }
        
    
    } catch (boost::system::system_error& e) {
        std::cerr << e.what() << std::endl;
    }
    activeConnection = false;
}

