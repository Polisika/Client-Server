#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <iostream> 
#include <sstream>
#include <fstream>
#include <string>
using namespace std;
#pragma region Functions
bool checkExistence(const char* filename)
{
	ifstream Infield(filename);
	return Infield.good();
}
string parsefileName(string str)
{
	int i;
	for (i = 0; str.c_str()[i] != -52; i++);
	string res = str.substr(0, i) + '\0';
	return res;
}
#pragma endregion
int main(void)
{
	#pragma region Init server
	WORD sockVer;
	WSADATA wsaData;
	int retVal;
	sockVer = MAKEWORD(2, 2);
	WSAStartup(sockVer, &wsaData);

	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (servSock == INVALID_SOCKET)
	{
		printf("Unable to create socket\n");
		WSACleanup();
		system("pause");
		return SOCKET_ERROR;
	}

	SOCKADDR_IN sin;
	sin.sin_family = PF_INET;
	sin.sin_port = htons(3114);
	sin.sin_addr.s_addr = INADDR_ANY;

	retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
	if (retVal == SOCKET_ERROR)
	{
		printf("Unable to bind\n");
		WSACleanup();
		system("pause");
		return SOCKET_ERROR;
	}
	printf("Server started at %s, port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
#pragma endregion
	#pragma region Server logic
	while (true)
	{
		retVal = listen(servSock, 10);
		if (retVal == SOCKET_ERROR)
		{
			printf("Unable to listen\n");
			WSACleanup();
			system("pause");
			return SOCKET_ERROR;
		}

		SOCKET clientSock;
		SOCKADDR_IN from;
		int fromlen = sizeof(from);
		clientSock = accept(servSock, (struct sockaddr*) & from, &fromlen);
		if (clientSock == INVALID_SOCKET)
		{
			printf("Unable to accept\n");
			WSACleanup();
			system("pause");
			return SOCKET_ERROR;
		}
		printf("New connection accepted from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
		char szReq[256];
		retVal = recv(clientSock, szReq, 256, 0);

		if (retVal == SOCKET_ERROR)
		{
			printf("Unable to recv\n");
			system("pause");
			return SOCKET_ERROR;
		}
		printf("Data received\n");
		string s = (const char*)szReq;
		if (s[0] == 's' && s[1] == 't')// Команда на выключение сервера
		{
			char szResp[] = "Server shutdown";
			retVal = send(clientSock, szResp, 256, 0);
			if (retVal == SOCKET_ERROR)
			{
				printf("Unable to recv\n");
				system("pause");
				return SOCKET_ERROR;
			}
			closesocket(clientSock);
			break;
		}
		else
		{
			char szResp[256];
			string fileName = parsefileName(s);

			if (!checkExistence(fileName.c_str()))
			{
				char err[] = "E";
				int ret = send(clientSock, err, sizeof(err), 0);
				if (ret == SOCKET_ERROR)
				{
					printf("Unable to send\n");
					system("pause");
					return SOCKET_ERROR;
				}
				closesocket(clientSock);
				continue;
			}
			else
			{
				FILE* in = fopen(fileName.c_str(), "rb");
				int bytes_read;
				char suc[] = "F";
				send(clientSock, suc, sizeof(suc), 0);

				while (!feof(in))
				{
					if ((bytes_read = fread(&szResp, 1, 256, in)) > 0)
						send(clientSock, szResp, bytes_read, 0);
					else
						break;
				}
				fclose(in);
			}
			closesocket(clientSock);
			printf("Connection closed\n");
		}
	}
	#pragma endregion
	#pragma region Cleanup
	closesocket(servSock);
	WSACleanup();
	#pragma endregion
	return 0;
}