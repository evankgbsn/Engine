#ifndef NetworkManager_H
#define NetworkManager_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

class NetworkManager
{
public:

	static void Initialize();

	static void Terminate();

private:

	NetworkManager();

	~NetworkManager();

	NetworkManager(const NetworkManager&) = delete;

	NetworkManager& operator=(const NetworkManager&) = delete;

	NetworkManager(NetworkManager&&) = delete;

	NetworkManager& operator=(NetworkManager&&) = delete;

	void InitializeWinsockServer();

	static NetworkManager* instance;

	const char* serverPort;

#ifdef _WIN32
	SOCKET serverIPV4SocketTCP;
	SOCKET serverIPV6SocketTCP;
#endif // _WIN32

};

#endif // NetworkManager_H