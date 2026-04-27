#include "Server.hpp"
#include "CommandDispatcher.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <utility>

volatile sig_atomic_t Server::_running = 1;

Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _serverFd(-1)
{
	std::memset(&_addr, 0, sizeof(_addr));
}

Server::~Server()
{
	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
	{
		close(it->first);
	}
	if (_serverFd != -1)
		close(_serverFd);
}


void	Server::signalHandler(int signum)
{
	(void)signum;
	_running = 0;
}

void	Server::_createSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << "Error: socket() failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error: setsockopt() failed" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl() failed" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}
}


void	Server::init()
{
	_createSocket();

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);

	if (bind(_serverFd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		std::cerr << "Error: bind() failed" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}

	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		std::cerr << "Error: listen() failed" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}

	struct pollfd serverPoll;
	serverPoll.fd = _serverFd;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfds.push_back(serverPoll);
	std::cout << "Server started on port " << _port << std::endl;
}

void	Server::run()
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGPIPE, SIG_IGN);

	CommandDispatcher dispatcher;

	while (_running)
	{
		int ret = poll(&_pollfds[0], _pollfds.size(), -1);
		if (ret == -1)
		{
			if (!_running)
				break ;
			std::cerr << "Error: poll() failed" << std::endl;
			break ;
		}

		if (_pollfds[0].revents & POLLIN)
			_acceptClient();

		for (size_t i = 1; i < _pollfds.size(); ++i)
		{
			if (_pollfds[i].revents & (POLLHUP | POLLERR))
			{
				_disconnectClient(_pollfds[i].fd);
				--i;
				continue ;
			}
			if (_pollfds[i].revents & POLLIN)
			{
				size_t oldSize = _pollfds.size();
				_handleClient(_pollfds[i].fd, dispatcher);
				if (_pollfds.size() < oldSize)
					--i;
			}
		}
	}
	std::cout << "\nServer stopped." << std::endl;
}

void	Server::_acceptClient()
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen = sizeof(clientAddr);

	int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd == -1)
	{
		std::cerr << "Error: accept() failed" << std::endl;
		return ;
	}

	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Error: fcntl() failed on client" << std::endl;
		close(clientFd);
		return ;
	}

	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	_pollfds.push_back(clientPoll);

	std::string host = inet_ntoa(clientAddr.sin_addr);
	_clients.insert(std::make_pair(clientFd, Client(clientFd, host)));

	std::cout << "New client connected (fd: " << clientFd << ")" << std::endl;
}

void	Server::_handleClient(int fd, CommandDispatcher &dispatcher)
{
	char	buffer[4096];
	int		bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		_disconnectClient(fd);
		return ;
	}
	if (bytesRead == 0)
	{
		_disconnectClient(fd);
		return ;
	}

	buffer[bytesRead] = '\0';

	Client *client = getClientByFd(fd);
	if (!client)
		return ;

	client->appendToBuffer(std::string(buffer, bytesRead));

	std::vector<std::string> messages = client->extractMessages();
	for (size_t i = 0; i < messages.size(); ++i)
	{
		std::cout << "Received [fd " << fd << "]: " << messages[i] << std::endl;
		dispatcher.dispatch(*this, *client, messages[i]);
		if (client->isMarkedForQuit())
		{
			_disconnectClient(fd);
			return ;
		}
	}
}

void	Server::_disconnectClient(int fd)
{
	std::cout << "Client disconnected (fd: " << fd << ")" << std::endl;

	removeClientFromChannels(fd);
	_clients.erase(fd);
	close(fd);

	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds.erase(_pollfds.begin() + i);
			break ;
		}
	}
}

int							Server::getPort() const { return _port; }
const std::string			&Server::getPassword() const { return _password; }
std::map<int, Client>		&Server::getClients() { return _clients; }
std::map<std::string, Channel>	&Server::getChannels() { return _channels; }

Client	*Server::getClientByFd(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return &(it->second);
	return NULL;
}

Client	*Server::getClientByNick(const std::string &nickname)
{
	for (std::map<int, Client>::iterator it = _clients.begin();
		 it != _clients.end(); ++it)
	{
		if (it->second.getNickname() == nickname)
			return &(it->second);
	}
	return NULL;
}

Channel	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return &(it->second);
	return NULL;
}

void	Server::addChannel(const std::string &name)
{
	_channels.insert(std::make_pair(name, Channel(name)));
}

void	Server::removeChannel(const std::string &name)
{
	_channels.erase(name);
}

void	Server::removeClientFromChannels(int fd)
{
	Client *client = getClientByFd(fd);
	std::string prefix;
	if (client && !client->getNickname().empty())
		prefix = client->getPrefix();
	else
		prefix = "*";

	std::vector<std::string> emptyChannels;

	for (std::map<std::string, Channel>::iterator it = _channels.begin();
		 it != _channels.end(); ++it)
	{
		if (it->second.isMember(fd))
		{
			std::string quitMsg = ":" + prefix + " QUIT :Connection lost\r\n";
			it->second.broadcast(quitMsg, fd);
			it->second.removeMember(fd);
			if (it->second.isEmpty())
				emptyChannels.push_back(it->first);
		}
	}

	for (size_t i = 0; i < emptyChannels.size(); ++i)
		_channels.erase(emptyChannels[i]);
}
