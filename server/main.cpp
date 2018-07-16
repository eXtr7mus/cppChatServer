// chit_chat.cpp: определяет точку входа для консольного приложения.
//

#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>
#include "tinyxml2.h"

#pragma comment(lib, "ws2_32.lib")

#include "CTcpListener.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif


void Listener_MessageReceived(CTcpListener *listener, int client, std::string msg);

int main(int argc, char* argv[])
{
	//using tinyxml2 for reading port and ipaddr from Config.xml	
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError err = doc.LoadFile("Config.xml");
	XMLCheckResult(err);
	tinyxml2::XMLNode *pRoot = doc.FirstChild();
	if (pRoot == nullptr) return tinyxml2::XML_ERROR_FILE_READ_ERROR;
	tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("port");
	if (pElement == nullptr) return tinyxml2::XML_ERROR_PARSING_ELEMENT;
	int port;
	err = pElement->QueryIntText(&port);
	XMLCheckResult(err);
	std::string ipaddr;
	pElement = pRoot->FirstChildElement("ipaddr");
	ipaddr = pElement->GetText();
	
	CTcpListener server(ipaddr, port, Listener_MessageReceived);


	
	if (server.Init()) {
		server.Run();
	}
}

void Listener_MessageReceived(CTcpListener *listener, int client, std::string msg) {
	listener->Send(client, msg);
}
