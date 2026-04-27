#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <map>
# include <poll.h>
# include <netinet/in.h>
# include <csignal>
# include <cstring>
# include <cstdlib>
# include <cerrno>
# include "Client.hpp"
# include "Channel.hpp"

class CommandDispatcher;

class Server
{
private:
	int							_port;
	std::string					_password;
	int							_serverFd;
	struct sockaddr_in			_addr;
	std::vector<struct pollfd>	_pollfds;
	std::map<int, Client>		_clients;

	std::map<std::string, Channel>	_channels;

	static volatile sig_atomic_t	_running;

	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);

	void	_createSocket();
	void	_acceptClient();
	void	_handleClient(int fd, CommandDispatcher &dispatcher);
	void	_disconnectClient(int fd);

public:
	Server(int port, const std::string &password);
	~Server();

	void	init();
	void	run();

	int							getPort() const;
	const std::string			&getPassword() const;

	std::map<int, Client>		&getClients();
	std::map<std::string, Channel>	&getChannels();

	Client						*getClientByFd(int fd);
	Client						*getClientByNick(const std::string &nickname);
	Channel						*getChannel(const std::string &name);

	void						addChannel(const std::string &name);
	void						removeChannel(const std::string &name);
	void						removeClientFromChannels(int fd);

	static void					signalHandler(int signum);
};

#endif
