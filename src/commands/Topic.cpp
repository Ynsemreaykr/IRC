#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief TOPIC komutunu işler.
 * 
 * Bir kanalın konusunu görüntülemek veya değiştirmek için kullanılır.
 * Eğer kanal 'konu kilidi' (+t) modundaysa, sadece operatörler konuyu değiştirebilir.
 * 
 * Kullanım: TOPIC <kanal> [<konu>]
 */
void	cmdTopic(Server &server, Client &client, std::vector<std::string> &params)
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
		client.sendReply(Reply::err_needmoreparams(nick, "TOPIC"));
		return ;
	}

	std::string channelName = params[0];

	// Kanalın varlığı kontrolü
	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	// Kullanıcı kanalda mı?
	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, channelName));
		return ;
	}

	// Eğer sadece kanal adı verildiyse, mevcut konuyu göster
	if (params.size() == 1)
	{
		if (channel->getTopic().empty())
			client.sendReply(Reply::rpl_notopic(nick, channelName));
		else
			client.sendReply(Reply::rpl_topic(nick, channelName, channel->getTopic()));
		return ;
	}

	// Konu değişikliği için yetki kontrolü (+t modu aktifse)
	if (channel->isTopicLocked() && !channel->isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(nick, channelName));
		return ;
	}

	// Yeni konuyu ayarla ve tüm kanala duyur
	std::string newTopic = params[1];
	channel->setTopic(newTopic);

	std::string topicMsg = ":" + client.getPrefix() + " TOPIC " +
						   channelName + " :" + newTopic + "\r\n";
	channel->broadcastAll(topicMsg);
}
