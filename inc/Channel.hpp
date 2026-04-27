#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>

class Client;

class Channel
{
private:
	std::string		_name;
	std::string		_topic;
	std::string		_key;
	std::set<int>	_members;
	std::set<int>	_operators;
	std::set<int>	_invited;
	bool			_inviteOnly;
	bool			_topicLock;
	int				_userLimit;

public:
	Channel();
	Channel(const std::string &name);
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);
	~Channel();

	const std::string		&getName() const;
	const std::string		&getTopic() const;
	const std::string		&getKey() const;
	bool					isInviteOnly() const;
	bool					isTopicLocked() const;
	int						getUserLimit() const;
	const std::set<int>		&getMembers() const;
	const std::set<int>		&getOperators() const;

	void					setTopic(const std::string &topic);
	void					setKey(const std::string &key);
	void					setInviteOnly(bool value);
	void					setTopicLock(bool value);
	void					setUserLimit(int limit);

	bool					isMember(int fd) const;
	bool					isOperator(int fd) const;
	bool					isInvited(int fd) const;
	bool					isEmpty() const;
	int						memberCount() const;

	void					addMember(int fd);
	void					removeMember(int fd);
	void					addOperator(int fd);
	void					removeOperator(int fd);
	void					addInvited(int fd);
	void					removeInvited(int fd);

	void					broadcast(const std::string &message, int excludeFd) const;
	void					broadcastAll(const std::string &message) const;

	std::string				getModeString() const;
};

#endif
