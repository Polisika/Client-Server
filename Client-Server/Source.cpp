// Для некоторых небезопасных операторов Си.
#define _CRT_SECURE_NO_WARNINGS
// Для функции inet_ntoa
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Подключаем динамическую библиотеку для работы с сокетами.
#pragma comment (lib,"Ws2_32.lib")
// Библиотека для работы с сокетами.
#include <winsock2.h>
// Подключаем для использования стандартных функций Си.
#include <stdio.h>
// Подключаем для стандартных методов для работы со стандартным вводом-выводом.
#include <iostream> 
// Для работы с файлами.
#include <fstream>
// Для работы со строками.
#include <string>
// По умолчанию используем пространство имён std.
using namespace std;
// Для удобного просмтотра кода в IDE делим его на логические блоки.
#pragma region Functions
// Проверка на существование файла в директории (или по указанному пути).
bool checkExistence(const char* filename)
{
	// Открываем файл с таким именем (по такому пути).
	ifstream Infield(filename);
	// Проверяем, открылся ли файл.
	return Infield.good();
}
/* Приведение строки к нормальному виду.
   Убираются русские символы 'М' из конца строки,
   необходимо отредактировать, если язык операционной системы будет другой.
*/
string parsefileName(string str)
{
	// Создание целочисленной переменной.
	int i;
	// Поиск последнего непустого символа.
	for (i = 0; str.c_str()[i] != -52; i++);
	// Берем подстроку - это и есть результат.
	string res = str.substr(0, i) + '\0';
	return res;
}
// Окончание логического блока.
#pragma endregion
int main(void)
{
	#pragma region Init server
	/* Инициализация WinSock */
	WORD sockVer; 			// Переменная для хранения версии динамической библиотеки.
	WSADATA wsaData;		// Переменная для хранения информации о загруженной библиотеке.
	int retVal;			// Целочисленная переменная для кодов операций.
	sockVer = MAKEWORD(2, 2);	// Версия сокета, которую мы хотим получить - 2.2 (MAKEWORD делает из двух 4-байтных чисел одно 8-ми байтовое).
	WSAStartup(sockVer, &wsaData);	// Инициализация библиотеки.

/*
	Создание серверного сокета.
	PF_INET - Internet протоколы
	SOCK_STREAM - потоковая передача пакетов, (есть ещё datagram и raw)
	(Этот тип обеспечивает последовательный, надежный, ориентированный на установление двусторонней связи поток байтов).
	IPPROTO_TCP - транспортный протокол TCP.
*/
	SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
/*
	Ошибка возникает при неполадках при создании сокета. Ошибки могут быть связаны с разными ситуациями.
	Например, не был проинициализирована библиотека функцией WSAStartup.
*/
	if (servSock == INVALID_SOCKET)
	{
		WSACleanup();
		// Сообщаем об ошибке.
		printf("Unable to create socket\n");
		// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
		system("pause");
		// Возвращаем код ошибки.
		return SOCKET_ERROR;
	}

	// Объявляем структуру для работы с протоколом IP.
	SOCKADDR_IN sin;
	// PF - семейство протоколов.
	sin.sin_family = PF_INET;
	/* htons нужен для приведения порта к единому виду, так как в интернете принято, что старший байт слева. */
	sin.sin_port = htons(2006);
	// Прослушиваем все адреса.
	sin.sin_addr.s_addr = INADDR_ANY;

	// Связывание сокета с IP и портом.
	retVal = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
	// Проверка на ошибку при связывании сокета.
	if (retVal == SOCKET_ERROR)
	{
		WSACleanup();
		// Сообщаем об ошибке.
		printf("Unable to bind\n");
		// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
		system("pause");
		// Возвращаем код ошибки.
		return SOCKET_ERROR;
	}
	// Сообщаем, что сервер принимает запросы.
	printf("Server started at %s, port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
	// Конец логического блока.
#pragma endregion
	#pragma region Server logic
	// Пока сервер не выключат.
	while (true)
	{
		// Теперь сокет ожидает запросы. 10 - сколько запросов может быть принято на один сокет.
		retVal = listen(servSock, 10);
		// Проверка на ошибку.
		// if - условный оператор, принимающий условия.
		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			// Сообщаем об ошибке.
			printf("Unable to listen\n");
			// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
			system("pause");
			// Возвращаем код ошибки.
			return SOCKET_ERROR;
		}
		// Создаем сокет клиента.
		SOCKET clientSock;
		// Переменная для связывания сокета с IP и портом.
		SOCKADDR_IN from;
		// Храним в целочисленной переменной размер структуры.
		int fromlen = sizeof(from);
		// Принимаем запрос от клиента.
		clientSock = accept(servSock, (struct sockaddr*) & from, &fromlen);
		if (clientSock == INVALID_SOCKET)
		{
			WSACleanup();
			// Сообщаем об ошибке.
			printf("Unable to accept\n");
			// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
			system("pause");
			// Возвращаем код ошибки.
			return SOCKET_ERROR;
		}
		printf("New connection accepted from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
		char szReq[1024];
		// Принимаем данные от клиента.
		retVal = recv(clientSock, szReq, 1024, 0);

		if (retVal == SOCKET_ERROR)
		{
			WSACleanup();
			// Сообщаем об ошибке.
			printf("Unable to recv\n");
			// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
			system("pause");
			// Возвращаем код ошибки.
			return SOCKET_ERROR;
		}
		printf("Data received\n");
		string s = (const char*)szReq;
		// Команда на отключание сервера. (имя файла не может начинаться с ':')
		if (s[0] == ':' && s[1] == 's')
		{
			char szResp[] = "Server shutdown";
			// Отсылаем данные клиенту.
			retVal = send(clientSock, szResp, 1024, 0);
			// Проверка на ошибку отправки.
			if (retVal == SOCKET_ERROR)
			{
				WSACleanup();
				// Сообщаем об ошибке.
				printf("Unable to recv\n");
				// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
				system("pause");
				// Возвращаем код ошибки.
				return SOCKET_ERROR;
			}
			// Закрываем сокет.
			closesocket(clientSock);
			// Перестаем принимать запросы.
			break;
		}
		else
		{
			char szResp[1024];
			// Приводим имя файла, которое прислал клиент к виду "файл.расширение\0"
			string fileName = parsefileName(s);
			// Проверка на существование файла.
			if (!checkExistence(fileName.c_str()))
			{
				// Файла не существует, необходимо оповестить клиента.
				char err[] = "E";
				int ret = send(clientSock, err, sizeof(err), 0);
				if (ret == SOCKET_ERROR)
				{
					WSACleanup();
					printf("Unable to send\n");
					system("pause");
					return SOCKET_ERROR;
				}
				closesocket(clientSock);
				// Продожаем принимать запросы.
				continue;
			}
			else
			{
				// Файл существует, открываем файл для чтения в байтовом представлении.
				FILE* in = fopen(fileName.c_str(), "rb");
				// Переменная для количества считанных байт.
				int bytes_read;
				// F - символ успеха.
				char suc[] = "F";
				// Сообщаем клиенту, что файл найден.
				send(clientSock, suc, sizeof(suc), 0);
				// Пока файл не был отправлен до конца.
				while (!feof(in))
				{
					// Если был считан хотя бы один байт, то отправляем пакет байт.
					if ((bytes_read = fread(&szResp, 1, 1024, in)) > 0)
						send(clientSock, szResp, bytes_read, 0);
					else
					// Иначе передача файла завершена.
						break;
				}
				// Закрываем файл.
				fclose(in);
			}
			// Закрываем сокет.
			closesocket(clientSock);
			// Пишем, что соединение закрыто.
			printf("Connection closed\n");
		}
	}
	// Конец логического блока.
	#pragma endregion
	// Начало логического блока.
	#pragma region Cleanup
	// Закрываем серверный сокет.
	closesocket(servSock);
	// Закрываем динамическую библиотеку.
	WSACleanup();
	// Конец логического блока.
	#pragma endregion
	// Программа завершена успешно.
	return 0;
}
