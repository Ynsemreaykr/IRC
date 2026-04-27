#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief Kanaldaki kullanıcıların listesini (NICK) oluşturur.
 * Operatörlerin başına '@' işareti ekler.
 */
static std::string	buildNamesList(Server &server, Channel &channel)
{
	std::string names;
	const std::set<int> &members = channel.getMembers();

	for (std::set<int>::const_iterator it = members.begin();
		 it != members.end(); ++it)
	{
		Client *c = server.getClientByFd(*it);
		if (!c)
			continue ;
		if (!names.empty())
			names += " ";
		if (channel.isOperator(*it))
			names += "@";
		names += c->getNickname();
	}
	return names;
}

/**
 * @brief JOIN komutunu işler.
 * 
 * Kullanıcının bir kanala girmesini sağlar. Kanal yoksa oluşturur ve
 * kullanıcıyı kanal operatörü yapar. Varsa, modlara (limit, key, invite-only) göre kontrol eder.
 * 
 * Kullanım: JOIN <kanal> [<anahtar>]
 */
void	cmdJoin(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	// Kayıt kontrolü
	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick));
		return ;
	}

	// Parametre kontrolü
	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "JOIN"));
		return ;
	}

	std::string channelName = params[0];
	std::string key = (params.size() > 1) ? params[1] : "";

	// Kanal adı '#' ile başlamalıdır
	if (channelName.empty() || channelName[0] != '#')
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	Channel *channel = server.getChannel(channelName);
	bool isNew = false;

	if (!channel)
	{
		// Kanal yoksa yeni oluştur
		isNew = true;
		server.addChannel(channelName);
		channel = server.getChannel(channelName);
	}
	else
	{
		// Kullanıcı zaten kanaldaysa bir şey yapma
		if (channel->isMember(client.getFd()))
			return ;

		// Davet usulü kontrolü (+i)
		if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
		{
			client.sendReply(Reply::err_inviteonlychan(nick, channelName));
			return ;
		}

		// Kanal şifresi kontrolü (+k)
		if (!channel->getKey().empty() && channel->getKey() != key)
		{
			client.sendReply(Reply::err_badchannelkey(nick, channelName));
			return ;
		}

		// Kullanıcı limiti kontrolü (+l)
		if (channel->getUserLimit() > 0 &&
			channel->memberCount() >= channel->getUserLimit())
		{
			client.sendReply(Reply::err_channelisfull(nick, channelName));
			return ;
		}
	}

	// Kullanıcıyı kanala ekle ve davetli listesinden temizle
	channel->addMember(client.getFd());
	channel->removeInvited(client.getFd());

	// Yeni oluşturulan kanalda ilk giren operatör olur
	if (isNew)
		channel->addOperator(client.getFd());

	// Katılım mesajını tüm kanala duyur
	std::string joinMsg = ":" + client.getPrefix() + " JOIN " + channelName + "\r\n";
	channel->broadcastAll(joinMsg);

	// Varsa kanal konusunu gönder
	if (!channel->getTopic().empty())
		client.sendReply(Reply::rpl_topic(nick, channelName, channel->getTopic()));

	// Kullanıcı listesini gönder (NAMES)
	std::string names = buildNamesList(server, *channel);
	client.sendReply(Reply::rpl_namreply(nick, channelName, names));
	client.sendReply(Reply::rpl_endofnames(nick, channelName));
}
