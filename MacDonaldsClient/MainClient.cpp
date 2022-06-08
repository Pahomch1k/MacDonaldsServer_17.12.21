#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

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
    int i = 0;
    sockaddr_in addrTo;
    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(23000);
    inet_pton(AF_INET, "10.4.4.165", &addrTo.sin_addr.s_addr);
    do {
        const size_t sendBufSize = 1024;
        char sendBuf[sendBufSize];
        gets_s(sendBuf, sendBufSize);
        cout << "Sending data...." << endl;
         
        int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&addrTo, sizeof(addrTo));
        if (sendResult == SOCKET_ERROR)
        {
            cout << "sendto failed with error " << WSAGetLastError() << endl;
            return 4;
        }

        const size_t receiveBufSize = 1024;
        char receiveBuf[receiveBufSize];

        sockaddr_in addrFrom;
        int addrFromSize = sizeof(addrFrom);

        cout << "Receiving data...." << endl;
        int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&addrFrom, &addrFromSize);

        if (bytesReceived == SOCKET_ERROR)
        {
            cout << "recvfrom failed with error " << WSAGetLastError() << endl;
            return 4;
        }

        receiveBuf[bytesReceived] = '\0';
        cout << "Received from " << addrFrom.sin_addr.S_un.S_addr << endl;
        cout << "Data: " << receiveBuf << endl;
        i++;
    } while (i < 10000);
    closesocket(udpSocket);
    WSACleanup();
}

