// Let's generate a simple server and client in C
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(SOCKET* client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_read;
    int message_count = 0;
    
    // Read client messages in a loop
    while ((bytes_read = recv(*client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received message %d: %s\n", message_count + 1, buffer);
        message_count++;
        
        // Send response
        const char* response = "Message received";
        send(*client_socket, response, strlen(response), 0);
        
        // If we've received 32 messages, we can stop
        if (message_count >= 32) {
            printf("Received all 64 messages\n");
            break;
        }
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            const char* goodbye = "Goodbye!";
            send(*client_socket, goodbye, strlen(goodbye), 0);
            break;
        }
    }
    
    if (bytes_read < 0) {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }
    
    printf("Total messages received: %d\n", message_count);
    closesocket(*client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept connection
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len)) == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }

        printf("Connection accepted from %s:%d\n", 
            inet_ntoa(client_addr.sin_addr), 
            ntohs(client_addr.sin_port));

        // Handle client in a new thread
        HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_client, 
                                   (LPVOID)&client_socket, 0, NULL);
        if (thread) {
            CloseHandle(thread);
        }
    }

    // Cleanup
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
