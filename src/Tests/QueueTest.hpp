#pragma once
#include "../defines.hpp"
#include "testBase.hpp"
#include "../Server.hpp"
#include "../Client.hpp"

namespace JNet {
    namespace test {
        TestResult queueTest(std::ostream& output) {
            using ClientPacket = JNet::udp::ReuseablePacket<JNet::udp::Packet<>, JNet::udp::bufferSize, false>;
            using ServerPacket = JNet::udp::ReuseablePacket<JNet::udp::Packet<>, JNet::udp::bufferSize, true>;
            TestResult testResult;
            testResult.tests = 2;
            testResult.succeded = 0;
            std::string host = "localhost";
            std::string port = "16632";
            std::string clientSendString = "Hello world!";
            std::string serverResponseString = "Hello there random stranger!";
            Server<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue> server(16632);
            JNet::Client<JNet::udp::Packet<>, JNet::udp::receiveMode::queue, JNet::udp::Packet<>, JNet::tcp::receiveMode::queue> client;


            server.run();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            client.connect(host, port);
            ClientPacket clientPacket = std::move(client.getPacket());
            memcpy(clientPacket.wrapper().getData(),clientSendString.data(),clientSendString.size());
            clientPacket.wrapper().setSize(clientSendString.size());
            clientPacket.wrapper().setMessageType(JNet::MessageType::Unset);
            clientPacket.wrapper().setId(0);
            client.sendPacket(std::move(clientPacket));

            std::this_thread::sleep_for(std::chrono::milliseconds(5));


            if (!server.hasAvailablePacket()) {
                output << fmt::format(fg(fmt::color::red), "Test failed! Serverqueue didn't receive packet\n");
                return testResult;
            }

            
            ServerPacket serverPacket = std::move(server.receiveIncomingPacket());
            std::string serverMessage((char*)serverPacket.wrapper().getData(), serverPacket.wrapper().getSize());

            if (serverMessage == clientSendString) {
                output << fmt::format(fg(fmt::color::light_green), "Test passed! Serverqueue received correct message\n");
                testResult.succeded++;
            } else {
                output << fmt::format(fg(fmt::color::red), "Test failed! Serverqueue received incorrect message:\n");
                output << serverMessage << "\n";
            }


            memcpy(serverPacket.wrapper().getData(),serverResponseString.data(),serverResponseString.size());
            serverPacket.wrapper().setSize(serverResponseString.size());
            server.sendPacket(std::move(serverPacket));
                
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            if (!client.hasAvailablePacket()) {
                output << fmt::format(fg(fmt::color::red), "Test failed! Clientqueue didn't receive packet\n");
                return testResult;
            }

            ClientPacket clientReceivedPacket = std::move(client.receiveIncomingPacket());
            std::string clientMessage((char*)clientReceivedPacket.wrapper().getData(), clientReceivedPacket.wrapper().getSize());

            if (clientMessage == serverResponseString) {
                output << fmt::format(fg(fmt::color::light_green), "Test passed! Clientqueue received correct message\n");
                testResult.succeded++;
            } else {
                output << fmt::format(fg(fmt::color::red), "Test failed! Clientqueue received incorrect message:\n");
                output << clientMessage << "\n";
            }

            client.disconnect();
            server.close();

            return testResult;
        }


        
    }
}

