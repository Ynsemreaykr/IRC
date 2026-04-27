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

/**
 * @brief IRC Sunucusunu yöneten ana sınıf.
 * 
 * Sunucunun başlatılması, istemci bağlantılarının kabul edilmesi,
 * soket yönetimi (poll), kanal ve istemci listelerinin tutulması
 * bu sınıfın sorumluluğundadır.
 */
class Server
{
private:
	int							_port;     // Sunucunun dinlediği port numarası
	std::string					_password; // Sunucuya giriş şifresi
	int							_serverFd; // Sunucu ana dinleme soketi (listening socket)
	struct sockaddr_in			_addr;     // Sunucu adres bilgileri yapısı
	std::vector<struct pollfd>	_pollfds;  // poll() fonksiyonu için kullanılan dosya tanımlayıcıları listesi
	std::map<int, Client>		_clients;  // Bağlı istemcileri fd üzerinden tutan harita

	std::map<std::string, Channel>	_channels; // Mevcut kanalları adları üzerinden tutan harita

	static volatile sig_atomic_t	_running; // Sunucunun çalışma durumunu kontrol eden sinyal bayrağı

	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);

	void	_createSocket(); // Sunucu soketini oluşturur ve yapılandırır
	void	_acceptClient(); // Yeni bir istemci bağlantısını kabul eder
	void	_handleClient(int fd, CommandDispatcher &dispatcher); // İstemciden gelen verileri işler
	void	_disconnectClient(int fd); // İstemci bağlantısını güvenli bir şekilde keser

public:
	Server(int port, const std::string &password);
	~Server();

	void	init(); // Sunucuyu hazırlar (soket oluşturma, bind, listen)
	void	run();  // Sunucu ana döngüsünü başlatır

	// Getter metodları
	int							getPort() const;        // Port numarasını döndürür
	const std::string			&getPassword() const;   // Sunucu şifresini döndürür

	std::map<int, Client>		&getClients();          // İstemci listesini döndürür
	std::map<std::string, Channel>	&getChannels();     // Kanal listesini döndürür

	// Arama ve yönetim metodları
	Client						*getClientByFd(int fd);                      // fd'ye göre istemci bulur
	Client						*getClientByNick(const std::string &nickname); // Takma ada göre istemci bulur
	Channel						*getChannel(const std::string &name);        // İsime göre kanal bulur

	void						addChannel(const std::string &name);         // Yeni bir kanal oluşturur
	void						removeChannel(const std::string &name);      // Bir kanalı siler
	void						removeClientFromChannels(int fd);            // İstemciyi tüm kanallardan çıkarır

	static void					signalHandler(int signum); // Sinyal yakalayıcı (SIGINT gibi)
};

#endif
