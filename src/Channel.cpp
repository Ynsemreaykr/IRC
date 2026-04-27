#include "Channel.hpp"
#include <sys/socket.h>

/**
 * @brief Varsayılan yapıcı (constructor). Modları varsayılan değerlere atar.
 */
Channel::Channel()
	: _inviteOnly(false), _topicLock(false), _userLimit(0)
{
}

/**
 * @brief İsim parametreli yapıcı. Belirtilen isimle bir kanal oluşturur.
 * @param name Kanalın adı
 */
Channel::Channel(const std::string &name)
	: _name(name), _inviteOnly(false), _topicLock(false), _userLimit(0)
{
}

/**
 * @brief Kopya yapıcı. Başka bir kanal nesnesinden veri kopyalar.
 */
Channel::Channel(const Channel &other)
	: _name(other._name), _topic(other._topic), _key(other._key),
	  _members(other._members), _operators(other._operators),
	  _invited(other._invited), _inviteOnly(other._inviteOnly),
	  _topicLock(other._topicLock), _userLimit(other._userLimit)
{
}

/**
 * @brief Atama operatörü.
 */
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

/**
 * @brief Yıkıcı (destructor).
 */
Channel::~Channel()
{
}

// Getter ve Setter implementasyonları
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

/**
 * @brief Belirtilen fd'nin kanala üye olup olmadığını kontrol eder.
 */
bool	Channel::isMember(int fd) const
{
	return _members.find(fd) != _members.end();
}

/**
 * @brief Belirtilen fd'nin kanal operatörü olup olmadığını kontrol eder.
 */
bool	Channel::isOperator(int fd) const
{
	return _operators.find(fd) != _operators.end();
}

/**
 * @brief Belirtilen fd'nin davetli listesinde olup olmadığını kontrol eder.
 */
bool	Channel::isInvited(int fd) const
{
	return _invited.find(fd) != _invited.end();
}

/**
 * @brief Kanalın boş olup olmadığını kontrol eder.
 */
bool	Channel::isEmpty() const
{
	return _members.empty();
}

/**
 * @brief Kanaldaki toplam üye sayısını döndürür.
 */
int		Channel::memberCount() const
{
	return static_cast<int>(_members.size());
}

/**
 * @brief Kanala yeni bir üye ekler.
 * @param fd Kullanıcının dosya tanımlayıcısı
 */
void	Channel::addMember(int fd)
{
	_members.insert(fd);
}

/**
 * @brief Kullanıcıyı kanaldan çıkarır. Operatör ve davetli listesinden de temizler.
 * @param fd Kullanıcının dosya tanımlayıcısı
 */
void	Channel::removeMember(int fd)
{
	_members.erase(fd);
	_operators.erase(fd);
	_invited.erase(fd);
}

/**
 * @brief Kullanıcıya operatör yetkisi verir.
 */
void	Channel::addOperator(int fd)
{
	_operators.insert(fd);
}

/**
 * @brief Kullanıcının operatör yetkisini alır.
 */
void	Channel::removeOperator(int fd)
{
	_operators.erase(fd);
}

/**
 * @brief Kullanıcıyı davetli listesine ekler.
 */
void	Channel::addInvited(int fd)
{
	_invited.insert(fd);
}

/**
 * @brief Kullanıcıyı davetli listesinden çıkarır.
 */
void	Channel::removeInvited(int fd)
{
	_invited.erase(fd);
}

/**
 * @brief Belirli bir kullanıcı hariç kanaldaki herkese mesaj gönderir.
 * @param message Gönderilecek mesaj
 * @param excludeFd Mesajın gönderilmeyeceği kullanıcı (genelde mesajı gönderen)
 */
void	Channel::broadcast(const std::string &message, int excludeFd) const
{
	for (std::set<int>::const_iterator it = _members.begin();
		 it != _members.end(); ++it)
	{
		if (*it != excludeFd)
			send(*it, message.c_str(), message.size(), 0);
	}
}

/**
 * @brief Kanaldaki herkese mesaj gönderir.
 * @param message Gönderilecek mesaj
 */
void	Channel::broadcastAll(const std::string &message) const
{
	for (std::set<int>::const_iterator it = _members.begin();
		 it != _members.end(); ++it)
	{
		send(*it, message.c_str(), message.size(), 0);
	}
}

/**
 * @brief Kanalın aktif modlarını (i, t, k, l) bir string olarak döndürür.
 */
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
