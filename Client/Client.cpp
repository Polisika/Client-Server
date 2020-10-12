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
// Для работы со строками.
#include <string>
// Подключаем для стандартных методов для работы со стандартным вводом-выводом.
#include <iostream>
// По умолчанию используем пространство имён std.
using namespace std;
// Для удобного просмтотра кода в IDE делим его на логические блоки.
#pragma region Functions
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

int main()
{
#pragma region Connect
   /* Инициализация WinSock */
   WORD ver = MAKEWORD(2, 2); // Переменная для хранения версии динамической библиотеки.
   WSADATA wsaData; // Переменная для хранения информации о загруженной библиотеке.
   int retVal = 0; // Целочисленная переменная для кодов операций.
   WSAStartup(ver, (LPWSADATA)&wsaData); // Инициализация библиотеки.
   
   /*
	Создание клиентского сокета.
	PF_INET - Internet протоколы
	SOCK_STREAM - потоковая передача пакетов, (есть ещё datagram и raw)
	(Этот тип обеспечивает последовательный, надежный, ориентированный на установление двусторонней связи поток байтов).
	IPPROTO_TCP - транспортный протокол TCP.
   */
   SOCKET clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   /*
	Ошибка возникает при неполадках при создании сокета. Ошибки могут быть связаны с разными ситуациями.
	Например, не был проинициализирована библиотека функцией WSAStartup.
   */
   if (clientSock == SOCKET_ERROR)
   {
      // Выгружаем динамическую библиотеку.
      WSACleanup();
		// Сообщаем об ошибке.
		printf("Unable to create socket\n");
		// Чтобы консоль не закрылась сразу, и человек успел прочитать что случилось.
		system("pause");
		// Возвращаем код ошибки.
		return SOCKET_ERROR;
   // Закрывающая скобка (обозначает конец логического блока).
   }
   // Запишем айпи в строку.
   string ip;
   // Приглашение для пользователя.
   cout << "ip>";
   // Записываем информацию с клавиатуры в эту переменную.
   cin >> ip;
   // Объявляем структуру для работы с протоколом IP.
   SOCKADDR_IN serverInfo;
   // PF - семейство протоколов.
   serverInfo.sin_family = PF_INET;
   // Указываем айпи по которому подключаемся.
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   /* htons нужен для приведения порта к единому виду, так как в интернете принято, что старший байт слева. */
   serverInfo.sin_port = htons(2006);
   // Подключаемся к серверу.
   retVal = connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));
   // Если ошибка
   if (retVal == SOCKET_ERROR)
   {
      // Уведомляем клиента.
      printf("Unable to connect\n");
      // Закрываем сокет.
      closesocket(clientSock);
      // Выгружаем динамическую библиотеку.
      WSACleanup();
      // Делаем так, чтобы консольне вылетала мгновенно при ошибке.
      system("pause");
      // Вернем код ошибки.
      return 1;
   }
   // Соединение произошло успешно.
   printf("Connection made sucessfully\n");
   // Конец логического блока.
#pragma endregion
   // Начало логического блока.
#pragma region Client Logic
   // Приглашение для пользователя.
   printf("Enter filename or ':s'\n");
   // Где будем хранить введенные данные.
   char pBuf[1024];
   // Вводим информацию с клавиатуры.
   gets_s(pBuf, 1024);
   // Уведомляем пользователя, что мы отправляем данные.
   printf("Sending request from client\n");
   // Отправляем данные.
   retVal = send(clientSock, (char*)pBuf, strlen((char*)pBuf), 0);
   // Если ошибка
   if (retVal == SOCKET_ERROR)
   {
      // Уведомляем клиента.
      printf("Unable to send\n");
      // Закрываем сокет.
      closesocket(clientSock);
      // Выгружаем динамическую библиотеку.
      WSACleanup();
      // Делаем так, чтобы консольне вылетала мгновенно при ошибке.
      system("pause");
      // Вернем код ошибки.
      return 1;
   }
   // Массив символов для принятия ответа.
   char szResponse[1024];
   // Должно прийти 'E' - ошибка или 'F' - успех.
   retVal = recv(clientSock, szResponse, 1024, 0);
   // Если команда выключения, то выключаем.
   if (pBuf[0] == ':' && pBuf[1] == 's')
   {
      // Уведомляем клиента.
      printf("Server shutdown.");
      // Закрываем сокет.
      closesocket(clientSock);
      // Выгружаем динамическую библиотеку.
      WSACleanup();
      // Делаем так, чтобы консольне вылетала мгновенно при ошибке.
      system("pause");
      // Вернем код ошибки.
      return 1;
   }
   // Ошибка
   if (szResponse[0] == 'E')
   {
      // Уведомляем, что файл не найден.
      printf("File not found.\n");
      // Закрываем сокет.
      closesocket(clientSock);
      // Выгружаем библиотеку.
      WSACleanup();
      // Делаем так, чтобы консольне вылетала мгновенно при ошибке.
      system("pause");
      // Вернем код ошибки.
      return 1;
   }
   if (retVal == SOCKET_ERROR)
   {
      // Уведомляем клиента.
      printf("Unable to recv.");
      // Закрываем сокет.
      closesocket(clientSock);
      // Выгружаем динамическую библиотеку.
      WSACleanup();
      // Делаем так, чтобы консольне вылетала мгновенно при ошибке.
      system("pause");
      // Вернем код ошибки.
      return 1;
   }
   // Создаем целочисленную переменную.
   int i = 0;
   // Создаем строку для имени файла.
   string buff((char*)pBuf);
   // Присваиваем значение корректного имени файла.
   buff = parsefileName(buff);
   // Открываем файл в бинарном представлении (или создаем).
   FILE* fd = fopen(buff.c_str(), "wb");
   // Размер текущего пакета
   int datasize = -1;
   // Пока не завершится передача файла.
   while (datasize != 0)
   {
      // Принимаем пакеты от сервера.
      datasize = recv(clientSock, (char*)pBuf, sizeof(pBuf), 0);
      // Пишем в файл принятые байты.
      fwrite(&pBuf, 1, datasize, fd);
   }
   // Конец логического блока
#pragma endregion
   // Начало логического блока.
#pragma region Cleanup
   // Закрываем файл.
   fclose(fd);
   // Закрываем сокет.
   closesocket(clientSock);
   // Выгружаем библиотеку.
   WSACleanup();
   // конец логического блока.
#pragma endregion
   // Не закрываем сразу консоль.
   system("pause");
   // Программа успешно завершена.
   return 0;
}
