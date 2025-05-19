// Simple client that sends "Hello World" to server

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 512
#define BUFFER_SIZE2 32
#define BUFFER_SIZE3 4096

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Socket creation error\n");
        WSACleanup();
        return 1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    
    // Send Hello World message with a number
    char message[50];
    int number = 7 ;  // You can change this number as needed


    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    

    if (number < 3) {
        sprintf(message, "Hello World %d", number);
        send(sock, message, strlen(message), 0);
        printf("%s message sent\n", message);
        // Receive 32 values from server
        int values[BUFFER_SIZE];
        int bytes_received = recv(sock, (char*)values, sizeof(values), 0);
        
        if (bytes_received == SOCKET_ERROR) {
            printf("recv failed with error: %d\n", WSAGetLastError());
        } else {
            printf("Received %d bytes of data\n", bytes_received);
            printf("Values received: ");
            for (int i = 0; i < bytes_received / sizeof(int); i++) {
                printf("%d ", values[i]);
            }
            printf("\n");
        }
    }
    else if (number == 3) {
        sprintf(message, "Hello World %d", number);
        send(sock, message, strlen(message), 0);
        printf("%s message sent\n", message);
        
        // Receive pattern data from server
        char pattern_data[2] = {0};
        int bytes_received = recv(sock, pattern_data, 2, 0);
        
        if (bytes_received == SOCKET_ERROR) {
            printf("recv failed with error: %d\n", WSAGetLastError());
        } else {
            printf("Received %d bytes of pattern data\n", bytes_received);
            printf("Pattern received: %c%c\n", pattern_data[0], pattern_data[1]);
        }
    }
    else if (number == 4) {
        sprintf(message, "Hello World %d", number);
        send(sock, message, strlen(message), 0);
        printf("%s message sent\n", message);
        
        // Receive pattern data from server (a, b, 1, 2)
        char pattern_data[BUFFER_SIZE2] = {0};
        int bytes_received = recv(sock, pattern_data, BUFFER_SIZE2, 0); // Receive all data from buffer
        if (bytes_received == SOCKET_ERROR) {
            printf("recv failed with error: %d\n", WSAGetLastError());
        } else {
            printf("Received %d bytes of pattern data\n", bytes_received);
            printf("Pattern received: %c%c%c%c%c\n", pattern_data[0], pattern_data[1], pattern_data[2], pattern_data[3], pattern_data[4]);
        }
    }
    else if (number > 5 && number < 8) {
        // Send Hello World 7 message 7 times
        sprintf(message, "Hello World %d", number);
        
        int successful_cycles = 0;
        for (int i = 0; i < BUFFER_SIZE3; i++) {
            printf("Attempt %d of BUFFER_SIZE3:\n", i + 1);
            
            // Create a new socket for each attempt to avoid connection reset issues
            if (i > 0) {
                closesocket(sock);
                if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
                    printf("Socket recreation error\n");
                    break;
                }
                
                // Reconnect to server
                if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                    printf("Reconnection Failed on attempt %d\n", i + 1);
                    continue;
                }
            }
            
            // Send the message
            if (send(sock, message, strlen(message), 0) == SOCKET_ERROR) {
                printf("Send failed with error: %d\n", WSAGetLastError());
                continue;
            }
            printf("%s message sent\n", message);
            
            // Receive pattern data from server
            char pattern_data[BUFFER_SIZE3] = {0};
            int bytes_received = recv(sock, pattern_data, BUFFER_SIZE3, 0);
            
            if (bytes_received == SOCKET_ERROR) {
                printf("recv failed with error: %d\n", WSAGetLastError());
                continue;
            } else {
                printf("Received %d bytes of pattern data\n", bytes_received);
                printf("Pattern received: %s\n", pattern_data);
                successful_cycles++;
            }
            
            // Small delay between attempts
            //Sleep(100);
        }
        
        printf("Completed %d successful send/receive cycles out of 7\n", successful_cycles);
    } 
    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
