#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main()
{
	int server_fd, new_socket;
	sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	std::string buffer(BUFFER_SIZE, 0);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		std::cerr << "Failed to create Socket!\n";
		exit(EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		std::cerr << "setsockopt() failed!\n";
		exit(EXIT_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "bind() faild!\n";
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0)
	{
		std::cerr << "listen() failed!\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Server listening on " << PORT << '\n';

	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		std::cerr << "Accept ain't accepting!\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Connection accepted! WOHOOOO...\n";

	// Reading data
	
	ssize_t valread;

	while (true)
	{
		valread = read(new_socket, &buffer[0], BUFFER_SIZE - 1);
		
		if (valread < 0)
		{
			std::cerr << "Read failed!!\n";
			break;
		}
		else if (valread == 0)
		{
			std::cout << "Client disconnected!\n";
			break;
		}
		
		buffer[valread] = '\0';
		std::cout << "Client: " << buffer;

		if (buffer.find("POST") == 0)
		{
			std::cout << "\n\nIt was a POST reqeust!!\n\n";
		}
		else if (buffer.find("GET") == 0)
		{
			std::cout << "\n\nIt was a GET request!!\n\n";
		}
		
		const std::string message_to_send = "Hoi, I got the data!\n\n";
		std::string response = R"(HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: )";
		response.append(std::to_string(message_to_send.size()));
		response.append("\n\n\r" + message_to_send);

		send(new_socket, response.c_str(), response.size(), 0);
	}
	close(new_socket);
	close(server_fd);
	std::cout << '\n';
	return 0;
}
