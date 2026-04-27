#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <set>

class Client;

/**
 * @brief IRC Kanalını temsil eden sınıf.
 * 
 * Bu sınıf, kanalın adını, konusunu, anahtarını (şifre), üyelerini,
 * operatörlerini ve davetli listesini yönetir. Ayrıca kanal modlarını
 * (davet usulü, konu kilidi, kullanıcı limiti) saklar.
 */
class Channel
{
private:
	std::string		_name;        // Kanalın adı (örn: #genel)
	std::string		_topic;       // Kanalın konusu
	std::string		_key;         // Kanala giriş anahtarı (şifre)
	std::set<int>	_members;     // Kanaldaki üyelerin dosya tanımlayıcıları (fd)
	std::set<int>	_operators;   // Kanal operatörlerinin fd listesi
	std::set<int>	_invited;     // Kanala davet edilen kullanıcıların fd listesi
	bool			_inviteOnly;  // Sadece davetle girilebilir modu (+i)
	bool			_topicLock;   // Konuyu sadece operatörler değiştirebilir modu (+t)
	int				_userLimit;   // Kanalın maksimum kullanıcı sınırı (+l)

public:
	Channel();
	Channel(const std::string &name);
	Channel(const Channel &other);
	Channel &operator=(const Channel &other);
	~Channel();

	// Getter metodları
	const std::string		&getName() const;     // Kanal adını döndürür
	const std::string		&getTopic() const;    // Kanal konusunu döndürür
	const std::string		&getKey() const;      // Kanal şifresini döndürür
	bool					isInviteOnly() const; // Davet modu aktif mi?
	bool					isTopicLocked() const;// Konu kilitli mi?
	int						getUserLimit() const; // Kullanıcı limitini döndürür
	const std::set<int>		&getMembers() const;  // Üyelerin listesini döndürür
	const std::set<int>		&getOperators() const;// Operatörlerin listesini döndürür

	// Setter metodları
	void					setTopic(const std::string &topic); // Kanal konusunu ayarlar
	void					setKey(const std::string &key);     // Kanal şifresini ayarlar
	void					setInviteOnly(bool value);          // Davet modunu ayarlar
	void					setTopicLock(bool value);           // Konu kilidini ayarlar
	void					setUserLimit(int limit);            // Kullanıcı limitini ayarlar

	// Durum kontrol metodları
	bool					isMember(int fd) const;   // Kullanıcı üye mi?
	bool					isOperator(int fd) const; // Kullanıcı operatör mü?
	bool					isInvited(int fd) const;  // Kullanıcı davetli mi?
	bool					isEmpty() const;          // Kanal boş mu?
	int						memberCount() const;      // Üye sayısını döndürür

	// Üye yönetimi
	void					addMember(int fd);    // Kanala üye ekler
	void					removeMember(int fd); // Kanaldan üye çıkarır
	void					addOperator(int fd);  // Kanala operatör atar
	void					removeOperator(int fd);// Operatör yetkisini alır
	void					addInvited(int fd);   // Davetli listesine ekler
	void					removeInvited(int fd);// Davetli listesinden çıkarır

	// Mesaj iletimi
	void					broadcast(const std::string &message, int excludeFd) const; // Belirli biri hariç herkese mesaj gönderir
	void					broadcastAll(const std::string &message) const;             // Kanalın tamamına mesaj gönderir

	// Yardımcı metodlar
	std::string				getModeString() const; // Kanalın modlarını string olarak döndürür
};

#endif
