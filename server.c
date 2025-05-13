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

    // Read client message
    while ((bytes_read = recv(*client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
        if (buffer == "Hello from the Client!") {
            // Send response
            const char* response = "Hello from the Server!";
            send(*client_socket, response, strlen(response), 0);
        }
        else if (buffer == "exit") {
            // Send response
            const char* response = "Goodbye!";
            send(*client_socket, response, strlen(response), 0);
            break;
        }
        break; // Exit after one message exchange
    }

    if (bytes_read < 0) {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }

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
