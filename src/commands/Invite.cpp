#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief INVITE komutunu işler.
 * 
 * Bir kullanıcıyı belirli bir kanala davet eder. Kanal davet modundaysa (+i),
 * sadece kanal operatörleri davet gönderebilir.
 * 
 * Kullanım: INVITE <takma_ad> <kanal>
 */
void	cmdInvite(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	// Kullanıcı kayıtlı değilse hata döndür
	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	// Parametre sayısı kontrolü
	if (params.size() < 2)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "INVITE"));
		return ;
	}

	std::string targetNick = params[0];
	std::string channelName = params[1];

	// Kanalın varlığı kontrolü
	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	// Davet eden kişi kanalda mı?
	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, channelName));
		return ;
	}

	// Kanal davet modundaysa operatör kontrolü yap
	if (channel->isInviteOnly() && !channel->isOperator(client.getFd()))
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

	// Hedef kullanıcı zaten kanalda mı?
	if (channel->isMember(targetClient->getFd()))
	{
		client.sendReply(Reply::err_useronchannel(nick, targetNick, channelName));
		return ;
	}

	// Kullanıcıyı kanalın davetli listesine ekle
	channel->addInvited(targetClient->getFd());

	// Davet eden kişiye onay mesajı gönder
	client.sendReply(Reply::rpl_inviting(nick, targetNick, channelName));

	// Hedef kullanıcıya davet mesajını ilet
	std::string inviteMsg = ":" + client.getPrefix() + " INVITE " +
							targetNick + " " + channelName + "\r\n";
	targetClient->sendReply(inviteMsg);
}
