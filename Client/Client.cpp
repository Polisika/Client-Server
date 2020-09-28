#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <iostream> 
using namespace std;
#pragma region Functions
string parsefileName(string str)
{
   int i;
   for (i = 0; str.c_str()[i] != -52; i++);
   string res = str.substr(0, i) + '\0';
   return res;
}
#pragma endregion

int main()
{
   #pragma region Сonnect
   WORD ver = MAKEWORD(2, 2);
   WSADATA wsaData;
   int retVal = 0;
   WSAStartup(ver, (LPWSADATA)&wsaData);
   LPHOSTENT hostEnt;
   hostEnt = gethostbyname("localhost");
   if (!hostEnt)
   {
      printf("Unable to collect gethostbyname\n");
      WSACleanup();
      system("pause");
      return 1;
   }

   SOCKET clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (clientSock == SOCKET_ERROR)
   {
      printf("Unable to create socket\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   string ip;
   cout << "ip>";
   cin >> ip;
   cin.ignore();

   SOCKADDR_IN serverInfo;
   serverInfo.sin_family = PF_INET;
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   serverInfo.sin_port = htons(3114);

   retVal = connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to connect\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   printf("Connection made sucessfully\n");
#pragma endregion
   #pragma region Client Logic
   printf("Enter filename or 's'\n");
   TCHAR pBuf[256];
   gets_s((char*)pBuf, 256);
   printf("Sending request from client\n");
   //Отсылаем данные на сервер
   retVal = send(clientSock, (char*)pBuf, strlen((char*)pBuf), 0);
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to send\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   char szResponse[256];

   retVal = recv(clientSock, szResponse, 256, 0);
   if (szResponse[0] == 'E')
   {
      printf("File not found.\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   if (retVal == SOCKET_ERROR)
   {
      printf("Unable to recv\n");
      WSACleanup();
      system("pause");
      return 1;
   }
   int i = 0;
   string buff((char*)pBuf);
   buff = parsefileName(buff);
   
   FILE* fd = fopen(buff.c_str(), "wb");
   int datasize = -1;
   while (datasize != 0)
   {
      datasize = recv(clientSock, (char*)pBuf, sizeof(pBuf), 0);
      fwrite(&pBuf, 1, datasize, fd);
   }
   #pragma endregion
   #pragma region Cleanup
   fclose(fd);
   closesocket(clientSock);
   WSACleanup();
   #pragma endregion
   system("pause");
   return 0;
}

