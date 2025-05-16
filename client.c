// Let's generate client in C

/*
For Windows systems:
First, you'll need to modify both files to use Windows networking:
1. Replace <arpa/inet.h> with <winsock2.h>
2. Replace <unistd.h> with <windows.h>
3. Add #pragma comment(lib, "ws2_32.lib")

To compile and run:

1. Open two Command Prompt windows

2. In the first terminal, compile and run the server:
   > gcc server.c -o server.exe -lws2_32
   > server.exe

3. In the second terminal, compile and run the client:
   > gcc client.c -o client.exe -lws2_32
   > client.exe

Note: If using MinGW, you might need to use:
   > gcc -o server.exe server.c -lws2_32
   > gcc -o client.exe client.c -lws2_32

The server will listen on port 8080 and the client will connect to localhost:8080
*/

#include <winsock2.h>
#include <ws2tcpip.h> // This includes inet_pton
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 64

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket - explicitly using TCP (SOCK_STREAM)
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Socket creation error\n");
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Send 64 messages to the server
    char message[64];
    for (int i = 1; i <= BUFFER_SIZE; i++) {
        sprintf(message, "%d", i);
        send(sock, message, strlen(message), 0);
        printf("Sent message %d\n", i);
    }
    
    printf("Waiting to receive 32 samples from server...\n");
    
    // Receive 32 samples from the server
    for (int i = 1; i <= 64; i++) {
        char response[BUFFER_SIZE] = {0};
        int bytes_received = recv(sock, response, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            response[bytes_received] = '\0';
            printf("Received sample %d: %s\n", i, response);
            
            // Send acknowledgment to server
            send(sock, "ACK", 3, 0);
        } else {
            printf("Failed to receive sample %d\n", i);
            break;
        }
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}



