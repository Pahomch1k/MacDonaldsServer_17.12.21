#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string.h> 
#include <string> 
#include <iostream> 
using namespace std;

int main()
{ 
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	WSADATA wsadata; 
	int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (res != NO_ERROR)
	{
		cout << "WSAStartup failked with error " << res << endl;
		return 1;
	}

	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET)
	{
		cout << "socket failed with error " << WSAGetLastError() << endl;
		return 2;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
	{
		cout << "bind failed with error " << WSAGetLastError() << endl;
		return 3;
	}

	//multicast
	char groupIp[255] = "235.0.0.0";

	ip_mreq mreq;
	inet_pton(AF_INET, groupIp, &mreq.imr_multiaddr.s_addr);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) != NO_ERROR)
	{
		cout << "setsockopt failed with error " << WSAGetLastError() << endl;
		return 3;
	} 

	sockaddr_in arr[1000];
	int i = 0; 
	int time = 0;
	int countGuests = 0;
	while (true) 
	{
		const size_t receiveBufSize = 1024;
		char receiveBuf[receiveBufSize];
		char ans[25]; 

		sockaddr_in senderAddr;
		int senderAddrSize = sizeof(senderAddr);
		int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

		bool flag = true;
		for (int j = 0; j < i; j++) if (arr[j].sin_port == senderAddr.sin_port) flag = false;

		if (flag)
		{
			arr[i] = senderAddr;
			i++;
		}

		const size_t sendBufSize = 1024;
		char sendBuf[sendBufSize];

		receiveBuf[bytesReceived] = '\0';  

		if (strcmp(receiveBuf, "1") == 0)
		{
			char buff[] = "What do you want?";
			strcpy_s(sendBuf, buff);

			for (int j = 0; j < i; j++)
			{
				int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&arr[j], senderAddrSize);
				if (sendResult == SOCKET_ERROR)
				{
					cout << "sendto failed with error " << WSAGetLastError() << endl;
					return 4;
				}
			}
		}
		else if (strcmp(receiveBuf, "2") == 0) exit(0);
		else if (strcmp(receiveBuf, "No") == 0) time = 0; 
		else if (strcmp(receiveBuf, "Yes") == 0 || strcmp(receiveBuf, "yes") == 0)
		{
			Sleep(time * 100);
			string Final = "Your zakaz gotov. Priyatnogog \nDo you wont again? \n1 - Zakaz \n2 - Exit";
			strcpy_s(sendBuf, Final.c_str());
			for (int j = 0; j < i; j++)
			{
				int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&arr[j], senderAddrSize);
				if (sendResult == SOCKET_ERROR)
				{
					cout << "sendto failed with error " << WSAGetLastError() << endl;
					return 4;
				}
			}
		} 
		else
		{
			time = 0;
			char charList1[1000];  
			char charList2[100];

			string stringMenu[6] = { "humburger", "sprite", "free", "bigmack", "cola", "roll"}; 
			int intMenu[6] = { 0, 0, 0, 0, 0, 0 }; 
			string Final = "Your zakaz ";

			for (int x = 0; x < 6; x++)
			{
				
				strcpy_s(charList2, stringMenu[x].c_str());
				char* pg12 = strstr(receiveBuf, charList2);
				
				if (pg12 != NULL)
				{
					string f{ charList2 }; 
					if (f == stringMenu[0])
					{
						time += 15;
						Final += stringMenu[0] + ", ";
					}
					else if (f == stringMenu[1])
					{
						time += 10;
						intMenu[1]++;
						Final += stringMenu[1] + ", ";
					}
					else if (f == stringMenu[2])
					{
						time += 10;
						intMenu[2]++;
						Final += stringMenu[2] + ", ";
					}
					else if (f == stringMenu[3])
					{
						time += 20;
						intMenu[3]++;
						Final += stringMenu[3] + ", ";
					}
					else if (f == stringMenu[4])
					{
						time += 10;
						intMenu[4]++;
						Final += stringMenu[4] + ", ";
					} 
					else if (f == stringMenu[5])
					{
						time += 30;
						intMenu[5]++;
						Final += stringMenu[5] + ", ";
					}
				} 
			}    
			string s = to_string(time);
			Final += "\nCook time - " + s;
			Final += "\nBudite jdat? Yes/No";
			
			strcpy_s(sendBuf, Final.c_str()); 
			 
			ofstream out("TimeQueue.txt"); 
			if (out.is_open())
			{ 
				for (int x = 0; x < 30; x++) out << time << endl; 
				out.close(); 
			}

			ifstream in("TimeQueue.txt");
			string str;
			int y = 0;
			while (!in.eof())
			{
				str = "";
				getline(in, str);  
			} 
			for (int j = 0; j < i; j++)
			{
				int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&arr[j], senderAddrSize);
				if (sendResult == SOCKET_ERROR)
				{
					cout << "sendto failed with error " << WSAGetLastError() << endl;
					return 4;
				}
			}  
		}  
	}
	 
	closesocket(udpSocket);
	WSACleanup();
}

