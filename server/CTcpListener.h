#pragma once

#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFF_SIZE (49000)

class CTcpListener;

//Callback to recieved
typedef void(*MessageReceivedHandler)(CTcpListener *listener, int socketId, std::string msg);

class CTcpListener
{
private:
	std::string m_ipAddress;
	int m_port;
	fd_set master;
	MessageReceivedHandler MessageHandler;
public:
	CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler);
	~CTcpListener();
	
	void Send(int clientSocket, std::string msg);

	bool Init();

	void Run();

	void Cleanup();

	SOCKET CreateSocket();

	SOCKET WaitForConnection(SOCKET listening);
};

