#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief PART komutunu işler.
 * 
 * Kullanıcının belirtilen kanaldan kendi isteğiyle ayrılmasını sağlar.
 * 
 * Kullanım: PART <kanal> [<sebep>]
 */
void	cmdPart(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	// Kayıt kontrolü
	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	// Parametre kontrolü
	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "PART"));
		return ;
	}

	std::string channelName = params[0];
	std::string reason = (params.size() > 1) ? params[1] : "";

	// Kanalın varlığı kontrolü
	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	// Kullanıcı gerçekten bu kanalda mı?
	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, channelName));
		return ;
	}

	// Ayrılma mesajını oluştur
	std::string partMsg = ":" + client.getPrefix() + " PART " + channelName;
	if (!reason.empty())
		partMsg += " :" + reason;
	partMsg += "\r\n";

	// Mesajı kanaldaki herkese (kendisi dahil) duyur
	channel->broadcastAll(partMsg);
	
	// Kullanıcıyı kanaldan çıkar
	channel->removeMember(client.getFd());

	// Eğer kanal boş kaldıysa kanalı sunucudan sil
	if (channel->isEmpty())
		server.removeChannel(channelName);
}
