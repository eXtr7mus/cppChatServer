// chit_chat.cpp: определяет точку входа для консольного приложения.
//

#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#include "CTcpListener.h"


void Listener_MessageReceived(CTcpListener *listener, int client, std::string msg);

int main(int argc, char* argv[])
{
	CTcpListener server("127.0.0.1", 54000, Listener_MessageReceived);

	if (server.Init()) {
		server.Run();
	}
}

void Listener_MessageReceived(CTcpListener *listener, int client, std::string msg) {
	listener->Send(client, msg);
}
