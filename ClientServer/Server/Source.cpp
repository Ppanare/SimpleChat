#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma execution_character_set("utf-8")
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <winsock2.h> 
#include <fstream>
#include <vector>
#include <string>
#include "../manch.h"

const std::string adress = "127.0.0.1";

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
		cout << "Panic!" << endl;
	}
}

void receiveFile(SOCKET socket) {
    streamsize fileSize;
    recv(socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
    vector<char> buffer(fileSize);
	recv(socket, buffer.data(), fileSize, 0);
    ofstream outputFile("./TRANZIT_DIRECTORY/received_file(CLIENT).txt", std::ios::binary);
    outputFile.write(buffer.data(), fileSize);

}



DWORD WINAPI serverReceive(LPVOID lpParam) { 
 char buffer[1024] = { 0 };
 SOCKET client = *(SOCKET*)lpParam; 
 while (true) { //Цикл работы сервера
  if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {

   cout << "recv function failed with error " << WSAGetLastError() << endl;
   return -1;
  }
  if (strcmp(buffer, "exit\n") == 0) {
   cout << "Client Disconnected." << endl;
   break;
  }
  if (strcmp(buffer, "send\n") == 0) { 
	  cout << "Catching file(CLIENT)";
	  receiveFile(client);
  }
  cout <<"Client: " << buffer << endl;
  memset(buffer, 0, sizeof(buffer)); 
 }
 return 1;
}

DWORD WINAPI serverSend(LPVOID lpParam) { 
 char buffer[1024] = { 0 };
 SOCKET client = *(SOCKET*)lpParam;
 while (true) {
  fgets(buffer, 1024, stdin);
  if (send(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
   cout << "send failed with error " << WSAGetLastError() << endl;
   return -1;
  }
  if (strcmp(buffer, "exit\n") == 0) {
   cout << "Thank you for using the application" << endl;
   break;
  }
  if (strcmp(buffer, "send\n") == 0) {
	  cout << "Send file" << endl;
	  sendFile("file.txt", client);
  }
 }
 return 1;
}

int main() {
 WSADATA WSAData;
 SOCKET server, client; 
 SOCKADDR_IN serverAddr, clientAddr; 
 int portAdress = 0;
 WSAStartup(MAKEWORD(2, 0), &WSAData);
 server = socket(AF_INET, SOCK_STREAM, 0); 
 if (server == INVALID_SOCKET) {
  cout << "Socket creation failed with error:" << WSAGetLastError() << endl;
  return -1;
 }
 serverAddr.sin_addr.s_addr = INADDR_ANY;
 serverAddr.sin_family = AF_INET;
 HANDLE colorConsole = GetStdHandle(STD_OUTPUT_HANDLE);
 SetConsoleTextAttribute(colorConsole, 3);
 cout << "\"SERVER\""<<" ip -> "<<adress<<endl;
 SetConsoleTextAttribute(colorConsole, 7);
 cout<<"Input number of socket" << endl;
 static int socketNumber = 0;
 std::cin >> socketNumber;
 serverAddr.sin_port = htons(socketNumber);
 if (bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
  cout << "Bind function failed with error: " << WSAGetLastError() << endl;
  return -1;
 }

 if (listen(server, 0) == SOCKET_ERROR) { 
  cout << "Listen function failed with error:" << WSAGetLastError() << endl;
  return -1;
 }
 cout << "Listening for incoming connections...." << endl; 

 char buffer[1024];
 int clientAddrSize = sizeof(clientAddr);
 if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
 
  cout << "Client connected! from "<<to_string(socketNumber)<< endl;
  cout << "Now you can use our live chat application. " << "Enter \"exit\" to disconnect" <<"\nOr type \"send\" to send file to TRANZIT_DIRECTORY" << endl; 

  DWORD tid; 
  HANDLE t1 = CreateThread(NULL, 0, serverReceive, &client, 0, &tid); 
  if (t1 == NULL) {
   cout << "Thread Creation Error: " << WSAGetLastError() << endl;
  }
  HANDLE t2 = CreateThread(NULL, 0, serverSend, &client, 0, &tid); 
  if (t2 == NULL) {
   cout << "Thread Creation Error: " << WSAGetLastError() << endl;
  }

  WaitForSingleObject(t1, INFINITE);
  WaitForSingleObject(t2, INFINITE);

  closesocket(client); 
  if (closesocket(server) == SOCKET_ERROR) { 
   cout << "Close socket failed with error: " << WSAGetLastError() << endl;
   return -1;
  }
  WSACleanup();
 }
}