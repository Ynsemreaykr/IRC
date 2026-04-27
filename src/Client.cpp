#include "Client.hpp"
#include <sys/socket.h>

Client::Client()
	: _fd(-1), _authenticated(false), _nickSet(false), _userSet(false),
	  _markedForQuit(false)
{
}

Client::Client(int fd, const std::string &hostname)
	: _fd(fd), _hostname(hostname),
	  _authenticated(false), _nickSet(false), _userSet(false),
	  _markedForQuit(false)
{
}

Client::Client(const Client &other)
	: _fd(other._fd), _nickname(other._nickname), _username(other._username),
	  _hostname(other._hostname),
	  _recvBuffer(other._recvBuffer), _authenticated(other._authenticated),
	  _nickSet(other._nickSet), _userSet(other._userSet),
	  _markedForQuit(other._markedForQuit)
{
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_nickname = other._nickname;
		_username = other._username;
		_hostname = other._hostname;
		_recvBuffer = other._recvBuffer;
		_authenticated = other._authenticated;
		_nickSet = other._nickSet;
		_userSet = other._userSet;
		_markedForQuit = other._markedForQuit;
	}
	return *this;
}

Client::~Client()
{
}

int					Client::getFd() const { return _fd; }
const std::string	&Client::getNickname() const { return _nickname; }
const std::string	&Client::getUsername() const { return _username; }
const std::string	&Client::getHostname() const { return _hostname; }
bool				Client::isAuthenticated() const { return _authenticated; }
bool				Client::isUserSet() const { return _userSet; }
bool				Client::isRegistered() const { return _nickSet && _userSet; }

void	Client::setNickname(const std::string &nickname) { _nickname = nickname; }
void	Client::setUsername(const std::string &username) { _username = username; }
void	Client::setAuthenticated(bool value) { _authenticated = value; }
void	Client::setNickSet(bool value) { _nickSet = value; }
void	Client::setUserSet(bool value) { _userSet = value; }

bool	Client::isMarkedForQuit() const { return _markedForQuit; }
void	Client::markForQuit() { _markedForQuit = true; }


void	Client::sendReply(const std::string &message) const
{
	if (_fd < 0)
		return ;
	send(_fd, message.c_str(), message.size(), 0);
}

void	Client::appendToBuffer(const std::string &data)
{
	_recvBuffer += data;
	if (_recvBuffer.size() > 8192)
		_recvBuffer.clear();
}

std::vector<std::string>	Client::extractMessages()
{
	std::vector<std::string>	messages;
	std::string::size_type		pos;

	while ((pos = _recvBuffer.find('\n')) != std::string::npos)
	{
		std::string line = _recvBuffer.substr(0, pos);
		_recvBuffer.erase(0, pos + 1);
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (!line.empty())
			messages.push_back(line);
	}
	return messages;
}

std::string	Client::getPrefix() const
{
	return _nickname + "!" + _username + "@" + _hostname;
}
