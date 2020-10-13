#include <iostream>
#include <sstream>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>

#define PORT "8000"
#define _WIN32_WINNT 0x501			// freeaddrinfo bind
#pragma comment(lib, "Ws2_32.lib")	// link dll to proj
using namespace std;

int main(void)
{
	bool isServerWorking = true;
	cout << "Setting server.." << endl << endl;
	cout << "_________________________________" << endl;
	WSAData wsaData; //мобель хранения информации о сокете
	cout << "Loading socket dll.." << endl;
	int res = WSAStartup(MAKEWORD(2,2), &wsaData); //загрузка dll сокетов в cpu
	// res = 0 -> good
	// res != 0 -> error
	if (res != 0) // библиотека не подгрузилась
	{
		return -1;
	}
	cout << "Loading socket dll completed!" << endl << endl;
	addrinfo* address = NULL; //данные об ip адресе
	cout << "Setting address.." << endl;
	//шаблон мобели данных об ip адресе
	addrinfo hints; 
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// инициализация address
	res = getaddrinfo("0.0.0.0", PORT, &hints, &address);

	if (res != 0) // не инициализировалась модель адреса
	{
		cout << WSAGetLastError() << endl;
		WSACleanup();
		return -2;
	}
	cout << "Setting address completed!" << endl << endl;
	cout << "Setting listen socket.." << endl;
	// создание сокета прослушки 
	int listen_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
	if (listen_socket == INVALID_SOCKET) 
	{
		cout << WSAGetLastError() << endl;
		freeaddrinfo(address);
		WSACleanup();
		return -3;
	}
	cout << "Setting listen socket completed!" << endl << endl;
	cout << "Binding address to listen socket.." << endl;
	res = bind(listen_socket, address->ai_addr, (int)address->ai_addrlen);

	if (res == SOCKET_ERROR) // не удалось привязать адрес к сокету
	{
		cout << WSAGetLastError() << endl;
		freeaddrinfo(address);
		closesocket(listen_socket);
		WSACleanup();
		return -4;
	}
	cout << "Binding address to listen socket completed!" << endl << endl;
	cout << "Setting server completed!" << endl << endl;
	int max_clients = SOMAXCONN;

	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &address, str, INET_ADDRSTRLEN);
	cout << "_________________________________" << endl;
	cout << "Dynamic host address:	" << "http://" << str << ":" << PORT << "/" << endl;
	cout << "Port:					" << PORT << endl;
	cout << "_________________________________" << endl;

	cout << "Starting listen requests.." << endl;
	res = listen(listen_socket, max_clients);
	if (res == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
		return -5;
	}
	
	while (isServerWorking)
	{
		cout << "Accepting new request.." << endl;
		// получение подключения
		int client_socket = accept(listen_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
		{
			closesocket(listen_socket);
			WSACleanup();
			return -6;
		}
		cout << "Accepting new request completed!" << endl << endl;
		cout << "Sending response.." << endl;
		const int max_buffer_size = 1024;
		char buffer[max_buffer_size];
		// получение содержимого зпроса
		res = recv(client_socket, buffer, max_buffer_size, 0);
		stringstream response;		//ответ сервера
		stringstream response_body;	//тело ответа сервера

		if (res == SOCKET_ERROR)
		{
			cout << WSAGetLastError() << endl;
			closesocket(client_socket);
		}
		else if (res == 0)
		{
			cout << WSAGetLastError() << endl;
			// клиент закрыл соединение
		}
		else if (res > 0)
		{
			// res - длина запроса
			if (res < 1024) buffer[res] = '\0';
			else buffer[1027] = '\0';
			response_body
				<< "<html>"
				<< "<p>test</p>"
				<< "</html>";
			response
				<< "HTTP/1.1 200 OK\r\n"
				<< "Version: HTTP/1.1\r\n"
				<< "Content-Type: text/html; charset=utf-8\r\n"
				<< "Content-Length: " << response_body.str().length()
				<< "\r\n\r\n"
				<< response_body.str();
			res = send(client_socket, response.str().c_str(), response.str().length(), 0);

			if (res == SOCKET_ERROR)
			{

			}
			closesocket(client_socket);
			cout << "Sending response completed!" << endl << endl;
		}
	}
	cout << "Stoping listen requests.." << endl;
	closesocket(listen_socket);
	freeaddrinfo(address);
	WSACleanup();
	cout << "Stoping listen requests completed!" << endl << endl;
	cout << "_________________________________" << endl;
	return 0;
}