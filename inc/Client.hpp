#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

/**
 * @brief IRC İstemcisini temsil eden sınıf.
 * 
 * Bu sınıf, sunucuya bağlı her bir kullanıcıyı temsil eder. Kullanıcının
 * bağlantı bilgilerini, kimlik bilgilerini ve mesaj tamponunu yönetir.
 */
class Client
{
private:
	int			_fd;           // İstemcinin soket dosya tanımlayıcısı (socket fd)
	std::string	_nickname;     // İstemcinin takma adı (nick)
	std::string	_username;     // İstemcinin kullanıcı adı
	std::string	_hostname;     // İstemcinin bağlandığı ana bilgisayar adı
	std::string	_recvBuffer;   // İstemciden gelen verilerin biriktirildiği tampon
	bool		_authenticated;// Şifre doğrulandı mı?
	bool		_nickSet;      // Takma ad ayarlandı mı?
	bool		_userSet;      // Kullanıcı bilgileri ayarlandı mı?
	bool		_markedForQuit;// İstemci bağlantısının kesilmesi için işaretlendi mi?

public:
	Client();
	Client(int fd, const std::string &hostname);
	Client(const Client &other);
	Client &operator=(const Client &other);
	~Client();

	// Getter metodları
	int					getFd() const;              // Soket fd'sini döndürür
	const std::string	&getNickname() const;       // Takma adı döndürür
	const std::string	&getUsername() const;       // Kullanıcı adını döndürür
	const std::string	&getHostname() const;       // Hostname'i döndürür
	bool				isAuthenticated() const;    // Kimlik doğrulaması yapıldı mı?
	bool				isUserSet() const;          // Kullanıcı bilgileri girildi mi?
	bool				isRegistered() const;       // İstemci tam olarak kayıtlı mı? (PASS, NICK, USER tamam mı?)

	// Setter metodları
	void				setNickname(const std::string &nickname); // Takma adı ayarlar
	void				setUsername(const std::string &username); // Kullanıcı adını ayarlar
	void				setAuthenticated(bool value);             // Kimlik doğrulama durumunu ayarlar
	void				setNickSet(bool value);                  // NICK ayarlandı bilgisini günceller
	void				setUserSet(bool value);                  // USER ayarlandı bilgisini günceller

	// Bağlantı yönetimi
	bool				isMarkedForQuit() const; // Çıkış için işaretlendi mi?
	void				markForQuit();           // İstemciyi çıkış için işaretler

	// İletişim metodları
	void				sendReply(const std::string &message) const; // İstemciye mesaj gönderir
	void				appendToBuffer(const std::string &data);     // Gelen veriyi tampona ekler
	std::vector<std::string>	extractMessages();                   // Tampondaki tam mesajları ayıklar

	// Yardımcı metodlar
	std::string			getPrefix() const; // İstemcinin prefix'ini döndürür (:nick!user@host)
};

#endif
