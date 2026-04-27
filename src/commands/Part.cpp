#include "Server.hpp"
#include "Reply.hpp"

void	cmdPart(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "PART"));
		return ;
	}

	std::string channelName = params[0];
	std::string reason = (params.size() > 1) ? params[1] : "";

	Channel *channel = server.getChannel(channelName);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, channelName));
		return ;
	}

	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, channelName));
		return ;
	}

	std::string partMsg = ":" + client.getPrefix() + " PART " + channelName;
	if (!reason.empty())
		partMsg += " :" + reason;
	partMsg += "\r\n";

	channel->broadcastAll(partMsg);
	channel->removeMember(client.getFd());

	if (channel->isEmpty())
		server.removeChannel(channelName);
}
