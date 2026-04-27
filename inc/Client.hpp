#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

class Client
{
private:
	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_hostname;
	std::string	_recvBuffer;
	bool		_authenticated;
	bool		_nickSet;
	bool		_userSet;
	bool		_markedForQuit;

public:
	Client();
	Client(int fd, const std::string &hostname);
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client();

	int					getFd() const;
	const std::string	&getNickname() const;
	const std::string	&getUsername() const;
	const std::string	&getHostname() const;
	bool				isAuthenticated() const;
	bool				isUserSet() const;
	bool				isRegistered() const;

	void				setNickname(const std::string &nickname);
	void				setUsername(const std::string &username);
	void				setAuthenticated(bool value);
	void				setNickSet(bool value);
	void				setUserSet(bool value);

	bool				isMarkedForQuit() const;
	void				markForQuit();

	void				sendReply(const std::string &message) const;
	void				appendToBuffer(const std::string &data);
	std::vector<std::string>	extractMessages();

	std::string			getPrefix() const;
};

#endif
