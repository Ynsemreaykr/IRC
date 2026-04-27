#include "CommandDispatcher.hpp"
#include "Client.hpp"
#include "Reply.hpp"

/**
 * @brief Yapıcı. Tüm IRC komutlarını ve bunlara karşılık gelen işleyici fonksiyonları kaydeder.
 */
CommandDispatcher::CommandDispatcher()
{
	registerCommand("PASS", cmdPass);
	registerCommand("NICK", cmdNick);
	registerCommand("USER", cmdUser);
	registerCommand("JOIN", cmdJoin);
	registerCommand("PART", cmdPart);
	registerCommand("QUIT", cmdQuit);
	registerCommand("PRIVMSG", cmdPrivmsg);
	registerCommand("KICK", cmdKick);
	registerCommand("INVITE", cmdInvite);
	registerCommand("TOPIC", cmdTopic);
	registerCommand("MODE", cmdMode);
}

/**
 * @brief Yıkıcı.
 */
CommandDispatcher::~CommandDispatcher()
{
}

/**
 * @brief Belirtilen isimdeki komutu ve işleyicisini haritaya ekler.
 */
void	CommandDispatcher::registerCommand(const std::string &name,
										   CommandHandler handler)
{
	_handlers[name] = handler;
}

/**
 * @brief String içerisindeki tüm karakterleri büyük harfe çevirir.
 */
std::string	CommandDispatcher::_toUpper(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::toupper(static_cast<unsigned char>(result[i]));
	return result;
}

/**
 * @brief IRC mesajlarını parçalarına (tokenlara) ayırır.
 * 
 * ':' ile başlayan son parametreyi (trailing parameter) tek bir parça olarak alır.
 * Örn: "PRIVMSG #kanal :Merhaba dünya" -> ["PRIVMSG", "#kanal", "Merhaba dünya"]
 */
std::vector<std::string>	CommandDispatcher::parseMessage(const std::string &message)
{
	std::vector<std::string>	tokens;
	std::string::size_type		i = 0;

	// Başındaki boşlukları atla
	while (i < message.size() && message[i] == ' ')
		++i;

	while (i < message.size())
	{
		// Eğer ':' ile başlıyorsa ve token listesi boş değilse, geri kalan her şeyi tek bir token yap
		if (message[i] == ':' && !tokens.empty()) 
		{
			tokens.push_back(message.substr(i + 1));
			return tokens;
		}

		std::string::size_type start = i;
		while (i < message.size() && message[i] != ' ')
			++i;
		tokens.push_back(message.substr(start, i - start));

		// Kelime aralarındaki boşlukları atla
		while (i < message.size() && message[i] == ' ')
			++i;
	}
	return tokens;
}

/**
 * @brief Gelen ham mesajı işleyerek uygun komut fonksiyonunu çağırır.
 */
void	CommandDispatcher::dispatch(Server &server, Client &client,
								   const std::string &rawMessage)
{
	std::vector<std::string> tokens = parseMessage(rawMessage);
	if (tokens.empty())
		return ;

	std::string command = _toUpper(tokens[0]);
	tokens.erase(tokens.begin()); // Komut adını parametrelerden ayır
	
	std::map<std::string, CommandHandler>::iterator it = _handlers.find(command);
	if (it != _handlers.end())
	{
		// Komut bulundu, işleyiciyi çağır
		it->second(server, client, tokens);
	}
	else
	{
		// Komut bulunamadı, hata mesajı gönder
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		client.sendReply(Reply::err_unknowncommand(nick, command));
	}
}
