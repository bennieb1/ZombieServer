// RakNet headers
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <RakSleep.h>
#include "../shared/Common.h"
#include "../shared/Vector.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(void)
{
    //
    // Get user input
    //
    std::string input;
    unsigned short serverPort = 1234;
    std::cout << "Enter the server port [default = 1234]: " << std::flush;
    getline(std::cin, input);
    if (!input.empty())
    {
        std::istringstream istr(input);
        istr >> serverPort;
    }

    // create the server interface
    RakNet::RakPeerInterface* server = RakNet::RakPeerInterface::GetInstance();

    // create a socket descriptor, which specifies which port and interface to listen on
    RakNet::SocketDescriptor sd(serverPort, NULL);

    // start the server
    if (server->Startup(8, &sd, 1) != RakNet::RAKNET_STARTED) 
	{
        std::cerr << "*** Failed to start server on port " << serverPort << std::endl;
        exit(1);
    }

    // set maximum number of incoming connections
    server->SetMaximumIncomingConnections(8);

    std::cout << "Listening on port " << serverPort << std::endl;
    std::cout << "IP addresses:" << std::endl;
	for (unsigned i = 0; i < server->GetNumberOfAddresses(); i++)
	{
		std::cout << server->GetLocalIP(i) << std::endl;
	}

    //
    // main loop
    //
    while (true) 
    {
        // get a packet
        RakNet::Packet* packet = server->Receive();

        while (packet != NULL) 
        {
            // We got a packet, get the identifier with our handy function
            unsigned char packetId = packet->data[0];

            switch (packetId) 
            {
                case ID_NEW_INCOMING_CONNECTION:
                    // Somebody connected.  We have their IP and port now
                    std::cout << "Got connection from " << packet->systemAddress.ToString(true) << std::endl;
                    break;

                case ID_DISCONNECTION_NOTIFICATION:
                    // Connection lost normally
                    std::cout << "Disconnected from " << packet->systemAddress.ToString(true) << std::endl;
                    break;

                case ID_CONNECTION_LOST:
                    // Couldn't deliver a reliable packet - i.e. the other system was abnormally terminated
                    std::cout << "Connection lost to " << packet->systemAddress.ToString(true) << std::endl;
                    break;

                case ID_INCOMPATIBLE_PROTOCOL_VERSION:
                    std::cerr << "Incomplatible protocol version from " << packet->systemAddress.ToString(true) << std::endl;
                    break;
                case GameMessages::ID_GAME_MESSAGE_POSITION:
					std::cout << "Got position packet from " << packet->systemAddress.ToString(true) << std::endl;
					break;
                case GameMessages::ID_GAME_MESSAGE_ROTATION:
                    std::cout << "Got rotation packet from " << packet->systemAddress.ToString(true) << std::endl;


                default:
                    std::cout << "Oops, received an unhandled packet with id " << (unsigned)packetId << std::endl;
                    break;
            }
          
            server->DeallocatePacket(packet);

            // get next packet
            packet = server->Receive();
        }

        // This sleep keeps RakNet responsive
        RakSleep(30);
    }

    server->Shutdown(300, 0, HIGH_PRIORITY);

    // we're done with the network
    RakNet::RakPeerInterface::DestroyInstance(server);

    return 0;
}
