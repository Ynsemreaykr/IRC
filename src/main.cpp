#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <cerrno>

static bool	isValidPort(const char *str, int &port)
{
	char	*endptr;

	errno = 0;
	long val = std::strtol(str, &endptr, 10);
	if (errno != 0 || *endptr != '\0' || val <= 0 || val > 65535)
		return false;
	port = static_cast<int>(val);
	return true;
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
		return EXIT_FAILURE;
	}
	int port;
	if (!isValidPort(argv[1], port))
	{
		std::cerr << "Error: Invalid port number (1-65535)" << std::endl;
		return EXIT_FAILURE;
	}
	std::string password = argv[2];
	if (password.empty())
	{
		std::cerr << "Error: Password cannot be empty" << std::endl;
		return EXIT_FAILURE;
	}
	Server server(port, password);
	server.init();
	server.run();

	return 0;
}
