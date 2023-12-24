#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <winsock2.h>
#include <string>
#include <fstream>
#include <vector>
#pragma comment(lib, "WS2_32.lib")
using namespace std;


void sendFile(const std::string& filePath, SOCKET socket) {
	ifstream file(filePath, ios::binary | ios::ate);
	if (!file.is_open()) {
		cout << "Error i cannot open this file" << endl;
		return;
	}
	streamsize fileSize = file.tellg();
	file.seekg(0, ios::beg);
	send(socket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);

	vector<char> buffer(fileSize);
	if (file.read(buffer.data(), fileSize)) {
		send(socket, buffer.data(), fileSize, 0);
	}
	else {
		cout << "FUCK" << endl;
	}
}

void receiveFile(SOCKET socket) {
	streamsize fileSize;
	recv(socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
	vector<char> buffer(fileSize);
	recv(socket, buffer.data(), fileSize, 0);
	ofstream outputFile("received_file"/*сюда путь можно ебануть*/, std::ios::binary);
	outputFile.write(buffer.data(), fileSize);

}

DWORD WINAPI clientReceive(LPVOID lpParam) { //Получение данных от сервера
 char buffer[1024] = { 0 };
 SOCKET server = *(SOCKET*)lpParam;
 while (true) {
  if (recv(server, buffer, sizeof(buffer), 0)  == SOCKET_ERROR) {
   cout << "recv function failed with error: " << WSAGetLastError() << endl;
   return -1;
  }
  if (strcmp(buffer, "exit\n") == 0) {
   cout << "Server disconnected." << endl;
   return 1;
  }
  cout << "Server: " << buffer << endl;
  memset(buffer, 0, sizeof(buffer));
 }
 return 1;
}

DWORD WINAPI clientSend(LPVOID lpParam) { //Отправка данных на сервер
 char buffer[1024] = { 0 };
 SOCKET server = *(SOCKET*)lpParam;
 while (true) {
  fgets(buffer, 1024, stdin);
  if (send(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
   cout << "send failed with error: " << WSAGetLastError() << endl;
   return -1;
  }
  if (strcmp(buffer, "exit") == 0) {
   cout << "Thank you for using the application" << endl;
   break;
  }
 }
 return 1;
}

int main() {
 WSADATA WSAData;
 SOCKET server;
 SOCKADDR_IN addr;
 WSAStartup(MAKEWORD(2, 0), &WSAData);
 if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
  cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
  return -1;
 }
 
 addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //коннект к серверу
 addr.sin_family = AF_INET;
 cout << "Input number of socket" << endl;
 int socketNumber = 0;
 std::cin >> socketNumber;
 addr.sin_port = htons(socketNumber);
 if (connect(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
  cout << "Server connection failed with error: " << WSAGetLastError() << endl;
  return -1;
 }
 
 cout << "Connected to server!" << endl;
 cout << "Now you can use our live chat application. "  << " Enter \"exit\" to disconnect" << endl;
 
 DWORD tid;
 HANDLE t1 = CreateThread(NULL, 0, clientReceive, &server, 0, &tid);
 if (t1 == NULL) cout << "Thread creation error: " << GetLastError();
 HANDLE t2 = CreateThread(NULL, 0, clientSend, &server, 0, &tid);
 if (t2 == NULL) cout << "Thread creation error: " << GetLastError();

 WaitForSingleObject(t1, INFINITE);
 WaitForSingleObject(t2, INFINITE);
 closesocket(server);
 WSACleanup();
}