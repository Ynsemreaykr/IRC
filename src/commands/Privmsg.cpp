#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief PRIVMSG komutunu işler.
 * 
 * Belirli bir kullanıcıya özel mesaj göndermek veya bir kanala genel mesaj
 * göndermek için kullanılır.
 * 
 * Kullanım: PRIVMSG <hedef> <mesaj>
 */
void	cmdPrivmsg(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	// Kayıt kontrolü
	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	// Alıcı (hedef) belirtilmiş mi?
	if (params.empty())
	{
		client.sendReply(Reply::err_norecipient(nick, "PRIVMSG"));
		return ;
	}

	// Mesaj içeriği var mı?
	if (params.size() < 2)
	{
		client.sendReply(Reply::err_notexttosend(nick));
		return ;
	}

	std::string target = params[0];
	std::string message = params[1];

	// Eğer hedef '#' ile başlıyorsa bir kanaldır
	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (!channel)
		{
			client.sendReply(Reply::err_nosuchchannel(nick, target));
			return ;
		}

		// Kanala mesaj göndermek için o kanalda olmak gerekir
		if (!channel->isMember(client.getFd()))
		{
			client.sendReply(Reply::err_cannotsendtochan(nick, target));
			return ;
		}

		// Mesajı hazırlayıp kanaldaki diğer herkese ilet
		std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " +
							  target + " :" + message + "\r\n";
		channel->broadcast(fullMsg, client.getFd());
	}
	else
	{
		// Hedef bir kullanıcıdır
		Client *targetClient = server.getClientByNick(target);
		if (!targetClient)
		{
			client.sendReply(Reply::err_nosuchnick(nick, target));
			return ;
		}

		// Mesajı hazırlayıp hedef kullanıcıya doğrudan gönder
		std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " +
							  target + " :" + message + "\r\n";
		targetClient->sendReply(fullMsg);
	}
}
