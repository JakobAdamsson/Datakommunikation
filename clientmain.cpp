#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string.h>
#include <iomanip>



// Enable if you want debugging to be printed, see example below.
// Alternatively, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG
#define ADD(a, b) ((a) + (b))
#define SUB(a, b) ((a) - (b))
#define MUL(a, b) ((a) * (b))
#define DIV(a, b) ((b) != 0 ? ((a) / (b)) : (std::cerr << "Div by zero" << std::endl, 0))


#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

struct sockaddr_in server_addr;

// Included to get the support library
#include <calcLib.h>



bool send_msg(char* msg, const char* dest, int port, int sockfd) {
    // Clear and set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(port); // Convert port to network byte order
    server_addr.sin_addr.s_addr = inet_addr(dest); // Convert destination IP address

    // connect
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sockfd);
        return false;
    }
    memset(buffer, 0, sizeof(buffer));
    int recieved_bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (recieved_bytes < 0) {
        std::cerr << "Failed to receive response" << std::endl;
        close(sockfd);
        return false;
    }

    // Send the message 1st
    if (send(sockfd, msg, strlen(msg), 0) < 0) {
        std::cerr << "Failed to send message" << std::endl;
        close(sockfd);
        return false;
    }

    buffer[recieved_bytes] = '\0';

    //std::cout << "ASSIGNMENT: " << buffer << std::endl;

    // Clear the buffer before use
    memset(buffer, 0, sizeof(buffer));
    int recieved_bytes2 = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

    if (recieved_bytes2 < 0) {
        std::cerr << "Failed to receive response" << std::endl;
        close(sockfd);
        return false;
    }
    std::cout << "ASSIGNMENT: " << buffer;

    return true;
}

std::vector<std::string> parse_message() {
    using namespace std;
    string buffer_as_string = buffer;

    stringstream ss(buffer_as_string);

    string temp;

    vector<std::string> split_buffer;

    while (ss >> temp) {
        split_buffer.push_back(temp);
    }

    return split_buffer;

}

int integer_operation(std::string op, int num1, int num2) {
    if (num2 <= 0) {
        return -1;
    }
    if (op == "add") {
        return ADD(num1, num2);
    }
    else if (op == "sub") {
        return SUB(num1, num2);
    }
    else if (op == "mul") {
        return MUL(num1, num2);
    }
    else if (op == "div") {
        return DIV(num1, num2);
    }
    else {
        return -1;
    }
}

float float_operation(std::string op, float  num1, float num2) {
    if (num2 <= 0) {
        return -1;
    }
    if (op == "fadd") {
        return ADD(num1, num2);
    }
    else if (op == "fsub") {
        return SUB(num1, num2);
    }
    else if (op == "fmul") {
        return MUL(num1, num2);
    }
    else if (op == "fdiv") {
        return DIV(num1, num2);
    }
    else {
        return -1;
    }
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <IP>:<Port>" << std::endl;
        return 1;
    }

    // Parse input <ip>:<port> syntax
    char delim[] = ":";
    char *Desthost = strtok(argv[1], delim);  // IP address part
    char *Destport = strtok(NULL, delim);     // Port part

    if (!Desthost || !Destport) {
        std::cerr << "Invalid input format. Expected <IP>:<Port>" << std::endl;
        return 1;
    }

    int port = atoi(Destport); // Convert port to integer

#ifdef DEBUG
    printf("Host: %s, Port: %d\n", Desthost, port);
#endif

    // Create socket
    // AF_INET = IPV4
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    char *msg = "OK\n";

    // Send the message
    if (!send_msg(msg, Desthost, port, sockfd)) {
        std::cerr << "Failed to send message" << std::endl;
        return 1;
    }
    std::vector parsed_msg = parse_message();
    int int1;
    int int2;
    int result1;
    bool result1_flag = false;

    float float1;
    float float2;
    float result2;
    bool result2_flag = false;
    if (parsed_msg[0] == "add" || parsed_msg[0] == "mul" || parsed_msg[0] == "div" || parsed_msg[0] == "sub") {
        int1 = stoi(parsed_msg[1]);
        int2 = stoi(parsed_msg[2]);
        result1 = integer_operation(parsed_msg[0], int1, int2);
        std::cout << "Calculated the result to " << result1 << std::endl;
        result1_flag = true;
    }
    else {
        float1 = std::stof(parsed_msg[1]);
        float2 = std::stof(parsed_msg[2]);
        result2 = float_operation(parsed_msg[0], float1, float2);
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Calculated the result to " << result2 << std::endl;
        result2_flag = true;
    }

    // Integer stuff
    int int_result;
    int recieved_bytes_result_int;

    // Float stuff
    float float_result;
    float recieved_bytes_result_float;

    memset(buffer, 0, sizeof(buffer));
    if (result1_flag) {
        std::string result_str = std::to_string(result1) + "\n";
        const char* int_char_thing = result_str.c_str();

        // Send the string representation of the integer
        int_result = send(sockfd, int_char_thing, strlen(int_char_thing), 0);
        if (int_result == -1) {
            std::cerr << "Error sending integer result." << std::endl;
        }


        int received_bytes_result_int = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received_bytes_result_int > 0) {
            buffer[received_bytes_result_int-1] = '\0';  // Null-terminate the received string

            std::cout << buffer << " (myresult=" << result1 << ")" << std::endl;
        } else if (received_bytes_result_int == 0) {
            std::cerr << "Connection closed by server." << std::endl;
        } else {
            std::cerr << "Error receiving response from the server." << std::endl;
        }
    }
    else if (result2_flag) {
        std::string result_str = std::to_string(result2) + "\n";
        const char* float_char_thing = result_str.c_str();

        float_result = send(sockfd, float_char_thing, strlen(float_char_thing), 0);
        if (float_result == -1) {
            std::cerr << "Error sending float result." << std::endl;
        }

        // Receive float result back (blocking call)
        int received_bytes_result_float = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (received_bytes_result_float > 0) {
            buffer[received_bytes_result_float-1] = '\0';  // Null-terminate the received string
            std::cout << buffer << " (myresult=" << result2 << ")" << std::endl;
        } else if (received_bytes_result_float == 0) {
            std::cerr << "Connection closed by server." << std::endl;
        } else {
            std::cerr << "Error receiving response from the server." << std::endl;
        }
    }

    close(sockfd);


    return 0;
}
