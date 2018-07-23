#include "CTcpListener.h"




CTcpListener::CTcpListener(std::string ipAddress, int port, MessageReceivedHandler handler):
m_ipAddress(ipAddress), m_port(port), MessageHandler(handler)
{
}


CTcpListener::~CTcpListener()
{
	Cleanup();
}

void CTcpListener::Send(int clientSocket, std::string msg) {
	send(clientSocket, msg.c_str(), msg.size() + 1, 0);
}

bool CTcpListener::Init() {
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsInit = WSAStartup(ver, &data);
	return wsInit == 0;
}

void CTcpListener::Run() {
	std::cout << "Server started on " << m_ipAddress << " on port " << m_port << std::endl;
	FD_ZERO(&master);
	char buff[MAX_BUFF_SIZE];
	SOCKET listening = CreateSocket();
	FD_SET(listening, &master);
	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				SOCKET client = WaitForConnection(listening);
				FD_SET(client, &master);
			} 
			else {
				ZeroMemory(buff, MAX_BUFF_SIZE);

				int bytesIn = recv(sock, buff, MAX_BUFF_SIZE, 0);
				if (bytesIn <= 0) {
					closesocket(sock);
					FD_CLR(sock, &master);
				} 
				else {
					for (int i = 0; i < master.fd_count; i++) {
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {
							std::ostringstream ss;
							ss << "SOCKET #" << sock << ": " << buff << "\r\n";
							std::string strOut = ss.str();
							std::cout << strOut << std::endl;
							if (MessageHandler != NULL) {
								MessageHandler(this, sock, strOut);
							}
						}
					}
				}
			}
		}
	
		
	}
	FD_CLR(listening, &master);
	closesocket(listening);
}

void CTcpListener::Cleanup() {
	while (master.fd_count > 0) {
		SOCKET sock = master.fd_array[0];
		FD_CLR(sock, &master);
		closesocket(sock);
	}
	WSACleanup();
}

SOCKET CTcpListener::CreateSocket() {
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening != INVALID_SOCKET) {
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(m_port);
		inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

		int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (bindOK != SOCKET_ERROR) {
			int listenOK = listen(listening, SOMAXCONN);
			if (listenOK == SOCKET_ERROR) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	return listening;
}

SOCKET CTcpListener::WaitForConnection(SOCKET listening) {
	sockaddr_in clientInf;
	int clientInfSize = sizeof(clientInf);
	SOCKET client = accept(listening, (sockaddr*)&clientInf, &clientInfSize);

	char buf[4096];
	ZeroMemory(buf, 4096);
	int bytesReceived = recv(client, buf, 4096, 0);

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " NAME : " << buf  << " connected on port " << service << std::endl;
	}
	else {
		inet_ntop(AF_INET, &clientInf.sin_addr, host, NI_MAXHOST);
		std::cout << host << " NAME : " << buf  << " connected on port " << ntohs(clientInf.sin_port) << std::endl;
	}

	std::ostringstream ss;
	ss << "Welcome to server, " << buf;

	std::string welcomeMsg = ss.str();
	send(client, welcomeMsg.c_str(), sizeof(welcomeMsg) + 1, 0);
	return client;
}
