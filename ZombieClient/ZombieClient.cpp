// RakNet headers
#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <RakSleep.h>
#include "../shared/Common.h"
#include "../shared/Vector.h"
#include <string>
#include <iostream>
#include <sstream>

#include <Kbhit.h>    // RakNet's portable implementation of the kbhit function

int main(void)
{
	std::string input;

	//
	// get user input
	//
	std::string serverIP;
	std::cout << "Enter the server IP [default = 127.0.0.1]: " << std::flush;
	getline(std::cin, input);
	if (input.empty())
	{
		serverIP = "127.0.0.1";
	}
	
	unsigned short serverPort = 1234;
	std::cout << "Enter the server port [default = 1234]: " << std::flush;
	getline(std::cin, input);
	if (!input.empty())
	{
		std::istringstream istr(input);
		istr >> serverPort;
	}

    // create client interface
	RakNet::RakPeerInterface *client = RakNet::RakPeerInterface::GetInstance();
	
    // create a socket descriptor
	RakNet::SocketDescriptor sd(0, NULL);

    // start the client interface
	client->Startup(1, &sd, 1);

    // connect to server
    if (client->Connect(serverIP.c_str(), serverPort, NULL, 0) != RakNet::CONNECTION_ATTEMPT_STARTED) 
	{
        std::cerr << "*** Failed to connect to server. Exiting." << std::endl;
        exit(1);
    }

    //
    // process packets
    //
    bool done = false;
    while (!done) 
	{
        RakNet::Packet* packet = client->Receive();

	    for ( ; packet != NULL; packet = client->Receive()) 
		{
		    // get the packet id
            unsigned char packetId = packet->data[0];

            // process the packet depending on the id
		    switch (packetId) 
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				
				Vector position(1.0f, 2.0f, 3.0f); // Example vector data
				
				sendPositionPacket(client, position);
				
					std::cout << "\nConnected to " << packet->systemAddress.ToString(true) << std::endl;

					break;

				case ID_CONNECTION_ATTEMPT_FAILED:
					std::cerr << "*** Connection attempt failed" << std::endl;
					done = true;
					break;

				case ID_NO_FREE_INCOMING_CONNECTIONS:
					// Sorry, the server is full.  I don't do anything here but
					// A real app should tell the user
					std::cerr << "*** No free incoming connections" << std::endl;
					done = true;
					break;

				case ID_INCOMPATIBLE_PROTOCOL_VERSION:
					std::cerr << "*** Incomplatible protocol version" << std::endl;
					done = true;
					break;

				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally
					std::cout << "*** Disconnected from " << packet->systemAddress.ToString(true) << std::endl;
					done = true;
					break;

				case ID_CONNECTION_LOST:
					// Couldn't deliver a reliable packet - i.e. the other system was abnormally terminated
					std::cout << "*** Connection lost to " << packet->systemAddress.ToString(true) << std::endl;
					done = true;
					break;

				case ID_USER_PACKET_ENUM:
					// show the fortune
					std::cout << "\n===== Here's your USER PACKET ======================" << std::endl;
					done = true;
					break;
		    }

            // cleanup
            client->DeallocatePacket(packet);
	    }

		if (kbhit())
		{
			std::string message;

			// read what the user typed (this blocks until the user hits ENTER)
			std::getline(std::cin, message);

			// We don't care what the user has done after they hit enter we will
			// send a packet
		}

        // This sleep keeps RakNet responsive
        RakSleep(30);
    }
    
	// be nice and let the server know we quit
    client->Shutdown(300, 0, HIGH_PRIORITY);

	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(client);

	return 0;
}
void sendPositionPacket(RakNet::RakPeerInterface* client, const Vector& position) {
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CONNECTION_REQUEST);
	position.Serialize(bsOut);
	client->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

// 