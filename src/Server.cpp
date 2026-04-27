#include "Server.hpp"
#include "CommandDispatcher.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <utility>

// Sunucunun çalışıp çalışmadığını tutan statik değişken (sinyal işleme için)
volatile sig_atomic_t Server::_running = 1;

/**
 * @brief Yapıcı. Port ve şifre bilgilerini saklar, adres yapısını temizler.
 */
Server::Server(int port, const std::string &password)
	: _port(port), _password(password), _serverFd(-1)
{
	std::memset(&_addr, 0, sizeof(_addr));
}

/**
 * @brief Yıkıcı. Tüm açık soketleri kapatarak kaynakları temizler.
 */
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

/**
 * @brief Sinyal yakalayıcı fonksiyon. Sunucuyu güvenli şekilde durdurmak için kullanılır (örn: Ctrl+C).
 */
void	Server::signalHandler(int signum)
{
	(void)signum;
	_running = 0;
}

/**
 * @brief Sunucu soketini oluşturur ve yapılandırır.
 * 
 * Soketi AF_INET ve SOCK_STREAM (TCP) olarak oluşturur, SO_REUSEADDR ayarını yapar
 * ve soketi bloklamayan (non-blocking) moda getirir.
 */
void	Server::_createSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << "Hata: socket() oluşturulamadı" << std::endl;
		exit(EXIT_FAILURE);
	}

	int opt = 1;
	// Portun hemen tekrar kullanılabilmesini sağlar
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Hata: setsockopt() başarısız" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}
	// Soketi bloklamayan moda geçir
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Hata: fcntl() başarısız" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}
}

/**
 * @brief Sunucuyu başlatmak için gerekli hazırlıkları yapar.
 * 
 * Soket oluşturur, adrese bağlar (bind) ve dinlemeye başlar (listen).
 * poll() için ana sunucu soketini listeye ekler.
 */
void	Server::init()
{
	_createSocket();

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);

	if (bind(_serverFd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
	{
		std::cerr << "Hata: bind() başarısız" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}

	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		std::cerr << "Hata: listen() başarısız" << std::endl;
		close(_serverFd);
		exit(EXIT_FAILURE);
	}

	struct pollfd serverPoll;
	serverPoll.fd = _serverFd;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;
	_pollfds.push_back(serverPoll);
	std::cout << "Sunucu " << _port << " portunda başlatıldı." << std::endl;
}

/**
 * @brief Sunucu ana döngüsünü çalıştırır.
 * 
 * poll() fonksiyonu ile tüm soketleri izler. Yeni bağlantı gelirse kabul eder,
 * mevcut istemcilerden veri gelirse işler. Sinyal gelene kadar döner.
 */
void	Server::run()
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGPIPE, SIG_IGN); // SIGPIPE sinyalini görmezden gel (kopan bağlantılar için)

	CommandDispatcher dispatcher;

	while (_running)
	{
		int ret = poll(&_pollfds[0], _pollfds.size(), -1);
		if (ret == -1)
		{
			if (!_running)
				break ;
			std::cerr << "Hata: poll() başarısız" << std::endl;
			break ;
		}

		// Yeni bağlantı kontrolü
		if (_pollfds[0].revents & POLLIN)
			_acceptClient();

		// Mevcut istemci soketlerini kontrol et
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
	std::cout << "\nSunucu durduruldu." << std::endl;
}

/**
 * @brief Yeni bir istemci bağlantısını kabul eder.
 */
void	Server::_acceptClient()
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen = sizeof(clientAddr);

	int clientFd = accept(_serverFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd == -1)
	{
		std::cerr << "Hata: accept() başarısız" << std::endl;
		return ;
	}

	// İstemci soketini bloklamayan moda geçir
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Hata: fcntl() istemci üzerinde başarısız" << std::endl;
		close(clientFd);
		return ;
	}

	// poll() listesine ekle
	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	_pollfds.push_back(clientPoll);

	// İstemci nesnesi oluştur ve haritaya ekle
	std::string host = inet_ntoa(clientAddr.sin_addr);
	_clients.insert(std::make_pair(clientFd, Client(clientFd, host)));

	std::cout << "Yeni istemci bağlandı (fd: " << clientFd << ")" << std::endl;
}

/**
 * @brief İstemciden gelen verileri okur ve işler.
 */
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
	if (bytesRead == 0) // İstemci bağlantıyı kapattı
	{
		_disconnectClient(fd);
		return ;
	}

	buffer[bytesRead] = '\0';

	Client *client = getClientByFd(fd);
	if (!client)
		return ;

	client->appendToBuffer(std::string(buffer, bytesRead));

	// Tampondan ayrıştırılmış her bir mesajı (satırı) işle
	std::vector<std::string> messages = client->extractMessages();
	for (size_t i = 0; i < messages.size(); ++i)
	{
		std::cout << "Alındı [fd " << fd << "]: " << messages[i] << std::endl;
		dispatcher.dispatch(*this, *client, messages[i]);
		if (client->isMarkedForQuit())
		{
			_disconnectClient(fd);
			return ;
		}
	}
}

/**
 * @brief İstemci bağlantısını keser ve temizlik yapar.
 */
void	Server::_disconnectClient(int fd)
{
	std::cout << "İstemci ayrıldı (fd: " << fd << ")" << std::endl;

	removeClientFromChannels(fd);
	_clients.erase(fd);
	close(fd);

	// poll() listesinden kaldır
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		if (_pollfds[i].fd == fd)
		{
			_pollfds.erase(_pollfds.begin() + i);
			break ;
		}
	}
}

// Getter implementasyonları
int							Server::getPort() const { return _port; }
const std::string			&Server::getPassword() const { return _password; }
std::map<int, Client>		&Server::getClients() { return _clients; }
std::map<std::string, Channel>	&Server::getChannels() { return _channels; }

/**
 * @brief fd'ye göre istemci nesnesini bulur.
 */
Client	*Server::getClientByFd(int fd)
{
	std::map<int, Client>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return &(it->second);
	return NULL;
}

/**
 * @brief Takma ada (nickname) göre istemci nesnesini bulur.
 */
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

/**
 * @brief İsime göre kanal nesnesini bulur.
 */
Channel	*Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return &(it->second);
	return NULL;
}

/**
 * @brief Sunucuya yeni bir kanal ekler.
 */
void	Server::addChannel(const std::string &name)
{
	_channels.insert(std::make_pair(name, Channel(name)));
}

/**
 * @brief Belirtilen kanalı sunucudan siler.
 */
void	Server::removeChannel(const std::string &name)
{
	_channels.erase(name);
}

/**
 * @brief Bir istemci sunucudan ayrıldığında, onu bulunduğu tüm kanallardan çıkarır.
 * Boş kalan kanalları siler.
 */
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
			std::string quitMsg = ":" + prefix + " QUIT :Bağlantı koptu\r\n";
			it->second.broadcast(quitMsg, fd);
			it->second.removeMember(fd);
			if (it->second.isEmpty())
				emptyChannels.push_back(it->first);
		}
	}

	// Boşalan kanalları temizle
	for (size_t i = 0; i < emptyChannels.size(); ++i)
		_channels.erase(emptyChannels[i]);
}
