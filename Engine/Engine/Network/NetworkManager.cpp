#include "NetworkManager.h"

#include "../Utils/Logger.h"

NetworkManager* NetworkManager::instance = nullptr;

void NetworkManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new NetworkManager();
		Logger::Log(std::string("Initialized NetworkManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling NetworkManager::Initialize before NetworkManager::Terminate"), Logger::Category::Warning);
	}
}

void NetworkManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated NetworkManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling NetworkManager::Terminate before NetworkManager::Initialize"), Logger::Category::Warning);
	}
}

NetworkManager::NetworkManager() :
	serverPort("27519")
#ifdef _WIN32
	,serverIPV4SocketTCP(INVALID_SOCKET),
	serverIPV6SocketTCP(INVALID_SOCKET)
#endif
{
#ifdef _WIN32
	InitializeWinsockServer();
#endif // _WIN32
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::InitializeWinsockServer()
{
	WSADATA wsaData;
	int iResult = -1;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	auto GetAddrInfoForFamily = [this, &iResult](unsigned int family) -> addrinfo*
	{
		addrinfo* result = nullptr;
		addrinfo* ptr = nullptr;
		addrinfo hints = {};

		hints.ai_family = family;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		iResult = getaddrinfo(nullptr, serverPort, &hints, &result);

		switch (iResult)
		{
		case 0:
			return result;
			break;
		case EAI_AGAIN:
			Logger::Log(std::string("A temporary failure in name resolution occurred."), Logger::Category::Error);
			break;
		case EAI_BADFLAGS:
			Logger::Log(std::string("An invalid value was provided to the ai_flags member of the pHints parameter."), Logger::Category::Error);
		case EAI_FAIL:
			Logger::Log(std::string("A nonrecoverable failure in name resolution occured."), Logger::Category::Error);
		case EAI_FAMILY:
			Logger::Log(std::string("An ai_family memeber of the pHints parameter is not supported."), Logger::Category::Error);
		case EAI_MEMORY:
			Logger::Log(std::string("A memory allocaton failure occurred."), Logger::Category::Error);
		case EAI_NONAME:
			Logger::Log(std::string("The name does not resolve for the supplied parameters or the pNodeNamme and pServiceNamee parameters were not provided."), Logger::Category::Error);
		case EAI_SERVICE:
			Logger::Log(std::string("The pServiceName parameter is not supported for the specified ai_socktype member of the pHints parameter."), Logger::Category::Error);
		case EAI_SOCKTYPE:
			Logger::Log(std::string("The ai_socktype member of the pHints parameter is not supported."), Logger::Category::Error);
		default:
			break;
		}
		return nullptr;
	};

	auto CreateSocket = [this](addrinfo* addrInfo) -> bool
	{
		if (addrInfo != nullptr)
		{
			serverIPV4SocketTCP = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
		}

		if (serverIPV4SocketTCP == INVALID_SOCKET)
		{
			Logger::Log(std::string("Failed to initialize ipv4 serve socket, NetworkManager::InitializeWinsock"), Logger::Category::Error);
			freeaddrinfo(addrInfo);
			return false;
		}
		else
		{
			return true;
		}
	};

	auto BindSockets = [this](addrinfo* ipv4, addrinfo* ipv6)
	{
		int results[2] =
		{
		 bind(serverIPV4SocketTCP, ipv4->ai_addr, (int)ipv4->ai_addrlen),
		 bind(serverIPV6SocketTCP, ipv6->ai_addr, (int)ipv6->ai_addrlen)
		};

		SOCKET sockets[2] =
		{
			serverIPV4SocketTCP,
			serverIPV6SocketTCP
		};

		addrinfo* addrInfos[2] =
		{
			ipv4,
			ipv6
		};

		int resultsLength = 1;
		bool cleanup = false;
		for (int result = results[resultsLength]; resultsLength >= 0; resultsLength--)
		{
			switch (result)
			{
			case WSANOTINITIALISED:
			case WSAENETDOWN:
			case WSAEACCES:
			case WSAEADDRINUSE:
			case WSAEADDRNOTAVAIL:
			case WSAEFAULT:
			case WSAEINPROGRESS:
			case WSAEINVAL:
			case WSAENOBUFS:
			case WSAENOTSOCK:
				closesocket(sockets[resultsLength]);
				cleanup = true;
				break;
			case !SOCKET_ERROR:

				break;
			default:
				break;
			}
			freeaddrinfo(addrInfos[resultsLength]);
			result = results[resultsLength];
		}

		if(cleanup)
			WSACleanup();
		
	};

	auto OnSuccess = [this, GetAddrInfoForFamily, CreateSocket, BindSockets]()
	{
		addrinfo* ipv4AddrInfo = GetAddrInfoForFamily(AF_INET);
		bool createdIPV4Socket = CreateSocket(ipv4AddrInfo);
		addrinfo* ipv6AddrInfo = GetAddrInfoForFamily(AF_INET6);
		bool createdIPV6Socket = CreateSocket(ipv4AddrInfo);

		if (createdIPV4Socket && createdIPV6Socket)
		{
			BindSockets(ipv4AddrInfo, ipv6AddrInfo);
			Logger::Log(std::string("Successfully initialized Winsock2"), Logger::Category::Success);
		}
		else
		{
			if (createdIPV4Socket)
				freeaddrinfo(ipv4AddrInfo);
			else if (createdIPV6Socket)
				freeaddrinfo(ipv6AddrInfo);

			WSACleanup();

			Logger::Log(std::string("Could not create both a IPV4 and IPV6 TCP server socket. NetworkManager::InitializeWinsock"), Logger::Category::Error);
		}
	};

	switch (iResult)
	{
	case 0:
		OnSuccess();
		break;
	case WSASYSNOTREADY: 
		Logger::Log(std::string("The underlying network subsystem is not ready for network communication."), Logger::Category::Error);
		break;
	case WSAVERNOTSUPPORTED:
		Logger::Log(std::string("The version of Windows Socets requested is not provided by this particular Windows sockets implementation."), Logger::Category::Error);
		break;
	case WSAEINPROGRESS:
		Logger::Log(std::string("A blocking windows sockets 1.1 operation is in progress."), Logger::Category::Error);
		break;
	case WSAEPROCLIM:
		Logger::Log(std::string("A limit on the number of tasks supported by the Windows Sockets implementation has been reached."), Logger::Category::Error);
		break;
	case WSAEFAULT:
		Logger::Log(std::string("The IpWSAData parameter is not a valid pointer."), Logger::Category::Error);
	default:
		break;
	}
}
