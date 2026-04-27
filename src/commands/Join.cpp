#include "Server.hpp"
#include "Reply.hpp"

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

void	cmdJoin(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "JOIN"));
		return ;
	}

	std::string channelName = params[0];
	std::string key = (params.size() > 1) ? params[1] : "";

	if (channelName.empty() || channelName[0] != '#')
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	Channel *channel = server.getChannel(channelName);
	bool isNew = false;

	if (!channel)
	{
		isNew = true;
		server.addChannel(channelName);
		channel = server.getChannel(channelName);
	}
	else
	{
		if (channel->isMember(client.getFd()))
			return ;

		if (channel->isInviteOnly() && !channel->isInvited(client.getFd()))
		{
			client.sendReply(Reply::err_inviteonlychan(nick, channelName));
			return ;
		}

		if (!channel->getKey().empty() && channel->getKey() != key)
		{
			client.sendReply(Reply::err_badchannelkey(nick, channelName));
			return ;
		}

		if (channel->getUserLimit() > 0 &&
			channel->memberCount() >= channel->getUserLimit())
		{
			client.sendReply(Reply::err_channelisfull(nick, channelName));
			return ;
		}
	}

	channel->addMember(client.getFd());
	channel->removeInvited(client.getFd());

	if (isNew)
		channel->addOperator(client.getFd());

	std::string joinMsg = ":" + client.getPrefix() + " JOIN " + channelName + "\r\n";
	channel->broadcastAll(joinMsg);

	if (!channel->getTopic().empty())
		client.sendReply(Reply::rpl_topic(nick, channelName, channel->getTopic()));

	std::string names = buildNamesList(server, *channel);
	client.sendReply(Reply::rpl_namreply(nick, channelName, names));
	client.sendReply(Reply::rpl_endofnames(nick, channelName));
}
