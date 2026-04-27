#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief KICK komutunu işler.
 * 
 * Bir kullanıcıyı kanaldan zorla çıkarmak için kullanılır. Sadece kanal operatörleri
 * bu komutu kullanabilir.
 * 
 * Kullanım: KICK <kanal> <kullanıcı> [<sebep>]
 */
void	cmdKick(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	// Kayıt kontrolü
	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	// Parametre kontrolü
	if (params.size() < 2)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "KICK"));
		return ;
	}

	std::string channelName = params[0];
	std::string targetNick = params[1];
	std::string reason = (params.size() > 2) ? params[2] : nick;

	// Kanalın varlığı kontrolü
	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	// Komutu gönderen kişi kanalda mı?
	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, channelName));
		return ;
	}

	// Komutu gönderen kişi operatör mü?
	if (!channel->isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(nick, channelName));
		return ;
	}

	// Hedef kullanıcı var mı?
	Client *targetClient = server.getClientByNick(targetNick);
	if (!targetClient)
	{
		client.sendReply(Reply::err_nosuchnick(nick, targetNick));
		return ;
	}

	// Hedef kullanıcı kanalda mı?
	if (!channel->isMember(targetClient->getFd()))
	{
		client.sendReply(Reply::err_usernotinchannel(nick, targetNick, channelName));
		return ;
	}

	// KICK mesajını tüm kanala duyur
	std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName +
						  " " + targetNick + " :" + reason + "\r\n";
	channel->broadcastAll(kickMsg);

	// Kullanıcıyı kanaldan çıkar
	channel->removeMember(targetClient->getFd());

	// Eğer kanal boş kaldıysa kanalı sil
	if (channel->isEmpty())
		server.removeChannel(channelName);
}
