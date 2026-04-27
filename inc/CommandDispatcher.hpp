#ifndef COMMANDDISPATCHER_HPP
# define COMMANDDISPATCHER_HPP

# include <string>
# include <vector>
# include <map>

class Server;
class Client;

/**
 * @brief Komut işleyici fonksiyon tipi tanımlaması.
 * 
 * Tüm IRC komutları bu imza yapısına sahip olmalıdır.
 * @param server Sunucu referansı
 * @param client Komutu gönderen istemci referansı
 * @param params Komutun parametre listesi
 */
typedef void (*CommandHandler)(Server &server, Client &client,
							   std::vector<std::string> &params);

/**
 * @brief IRC komutlarını ayrıştıran ve ilgili işleyicilere yönlendiren sınıf.
 * 
 * Bu sınıf, gelen ham mesajları parse eder, komut adını belirler
 * ve daha önce kaydedilmiş olan uygun fonksiyonu çağırır.
 */
class CommandDispatcher
{
private:
	std::map<std::string, CommandHandler>	_handlers; // Komut adları ve işleyici fonksiyonları eşleyen harita

	CommandDispatcher(const CommandDispatcher &other);
	CommandDispatcher &operator=(const CommandDispatcher &other);

	static std::string						_toUpper(const std::string &str); // String'i büyük harfe çevirir

public:
	CommandDispatcher();
	~CommandDispatcher();

	void	registerCommand(const std::string &name, CommandHandler handler); // Yeni bir komutu sisteme kaydeder
	void	dispatch(Server &server, Client &client,
					 const std::string &rawMessage); // Ham mesajı işleyip ilgili komuta yönlendirir

	static std::vector<std::string>	parseMessage(const std::string &message); // Mesajı parçalarına ayırır
};

// Komut işleyici fonksiyon prototipleri
void	cmdPass(Server &server, Client &client, std::vector<std::string> &params);     // PASS komutu
void	cmdNick(Server &server, Client &client, std::vector<std::string> &params);     // NICK komutu
void	cmdUser(Server &server, Client &client, std::vector<std::string> &params);     // USER komutu
void	cmdJoin(Server &server, Client &client, std::vector<std::string> &params);     // JOIN komutu
void	cmdPart(Server &server, Client &client, std::vector<std::string> &params);     // PART komutu
void	cmdQuit(Server &server, Client &client, std::vector<std::string> &params);     // QUIT komutu
void	cmdPrivmsg(Server &server, Client &client, std::vector<std::string> &params);  // PRIVMSG komutu
void	cmdKick(Server &server, Client &client, std::vector<std::string> &params);     // KICK komutu
void	cmdInvite(Server &server, Client &client, std::vector<std::string> &params);   // INVITE komutu
void	cmdTopic(Server &server, Client &client, std::vector<std::string> &params);    // TOPIC komutu
void	cmdMode(Server &server, Client &client, std::vector<std::string> &params);     // MODE komutu

#endif
