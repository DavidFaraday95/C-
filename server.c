// Simple server that only receives data
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 64
#define BUFFER_SIZE2 128

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
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

    // Continuously accept connections
    while (1) {
        // Accept connection
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len)) == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            continue; // Continue to next iteration instead of exiting
        }

        printf("Connection accepted from %s:%d\n", 
            inet_ntoa(client_addr.sin_addr), 
            ntohs(client_addr.sin_port));
        
        // Receive data from client
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);
            
            if (strcmp(buffer, "Hello World 1") == 0) {
            // Send 32 values to the client
                int values[BUFFER_SIZE];
                for (int i = 0; i < BUFFER_SIZE; i++) {
                    values[i] = i + 1; // Fill with values 1-32
                }
                
                // Send the array to client
                int bytes_sent = send(client_socket, (char*)values, sizeof(values), 0);
                if (bytes_sent == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                } else {
                    printf("Sent %d bytes of data to client\n", bytes_sent);
                }
            }   

            if (strcmp(buffer, "Hello World 2") == 0) {
            // Send 32 values to the client
                int values[BUFFER_SIZE2];
                for (int i = 0; i < BUFFER_SIZE2; i++) {
                    values[i] = i + 1; // Fill with values 1-32
                }
                
                // Send the array to client
                int bytes_sent = send(client_socket, (char*)values, sizeof(values), 0);
                if (bytes_sent == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                } else {
                    printf("Sent %d bytes of data to client\n", bytes_sent);
                }
            }
        }
        if (bytes_received == SOCKET_ERROR) {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
        
        printf("Connection closed\n");
        
        // Close client socket but keep server running
        closesocket(client_socket);
    }
    
    // Cleanup (this code will only be reached if the loop is broken)
    closesocket(server_socket);
    WSACleanup();
    return 0;
}