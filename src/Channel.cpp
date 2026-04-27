#include "Channel.hpp"
#include <sys/socket.h>

Channel::Channel()
	: _inviteOnly(false), _topicLock(false), _userLimit(0)
{
}

Channel::Channel(const std::string &name)
	: _name(name), _inviteOnly(false), _topicLock(false), _userLimit(0)
{
}

Channel::Channel(const Channel &other)
	: _name(other._name), _topic(other._topic), _key(other._key),
	  _members(other._members), _operators(other._operators),
	  _invited(other._invited), _inviteOnly(other._inviteOnly),
	  _topicLock(other._topicLock), _userLimit(other._userLimit)
{
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_key = other._key;
		_members = other._members;
		_operators = other._operators;
		_invited = other._invited;
		_inviteOnly = other._inviteOnly;
		_topicLock = other._topicLock;
		_userLimit = other._userLimit;
	}
	return *this;
}

Channel::~Channel()
{
}

const std::string	&Channel::getName() const { return _name; }
const std::string	&Channel::getTopic() const { return _topic; }
const std::string	&Channel::getKey() const { return _key; }
bool				Channel::isInviteOnly() const { return _inviteOnly; }
bool				Channel::isTopicLocked() const { return _topicLock; }
int					Channel::getUserLimit() const { return _userLimit; }
const std::set<int>	&Channel::getMembers() const { return _members; }
const std::set<int>	&Channel::getOperators() const { return _operators; }

void	Channel::setTopic(const std::string &topic) { _topic = topic; }
void	Channel::setKey(const std::string &key) { _key = key; }
void	Channel::setInviteOnly(bool value) { _inviteOnly = value; }
void	Channel::setTopicLock(bool value) { _topicLock = value; }
void	Channel::setUserLimit(int limit) { _userLimit = limit; }

bool	Channel::isMember(int fd) const
{
	return _members.find(fd) != _members.end();
}

bool	Channel::isOperator(int fd) const
{
	return _operators.find(fd) != _operators.end();
}

bool	Channel::isInvited(int fd) const
{
	return _invited.find(fd) != _invited.end();
}

bool	Channel::isEmpty() const
{
	return _members.empty();
}

int		Channel::memberCount() const
{
	return static_cast<int>(_members.size());
}

void	Channel::addMember(int fd)
{
	_members.insert(fd);
}

void	Channel::removeMember(int fd)
{
	_members.erase(fd);
	_operators.erase(fd);
	_invited.erase(fd);
}

void	Channel::addOperator(int fd)
{
	_operators.insert(fd);
}

void	Channel::removeOperator(int fd)
{
	_operators.erase(fd);
}

void	Channel::addInvited(int fd)
{
	_invited.insert(fd);
}

void	Channel::removeInvited(int fd)
{
	_invited.erase(fd);
}

void	Channel::broadcast(const std::string &message, int excludeFd) const
{
	for (std::set<int>::const_iterator it = _members.begin();
		 it != _members.end(); ++it)
	{
		if (*it != excludeFd)
			send(*it, message.c_str(), message.size(), 0);
	}
}

void	Channel::broadcastAll(const std::string &message) const
{
	for (std::set<int>::const_iterator it = _members.begin();
		 it != _members.end(); ++it)
	{
		send(*it, message.c_str(), message.size(), 0);
	}
}

std::string	Channel::getModeString() const
{
	std::string modes;

	if (_inviteOnly)
		modes += 'i';
	if (_topicLock)
		modes += 't';
	if (!_key.empty())
		modes += 'k';
	if (_userLimit > 0)
		modes += 'l';
	return modes;
}
