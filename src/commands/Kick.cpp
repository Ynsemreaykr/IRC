#include "Server.hpp"
#include "Reply.hpp"

void	cmdKick(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.size() < 2)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "KICK"));
		return ;
	}

	std::string channelName = params[0];
	std::string targetNick = params[1];
	std::string reason = (params.size() > 2) ? params[2] : nick;

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

	if (!channel->isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(nick, channelName));
		return ;
	}

	Client *targetClient = server.getClientByNick(targetNick);
	if (!targetClient)
	{
		client.sendReply(Reply::err_nosuchnick(nick, targetNick));
		return ;
	}

	if (!channel->isMember(targetClient->getFd()))
	{
		client.sendReply(Reply::err_usernotinchannel(nick, targetNick, channelName));
		return ;
	}

	std::string kickMsg = ":" + client.getPrefix() + " KICK " + channelName +
						  " " + targetNick + " :" + reason + "\r\n";
	channel->broadcastAll(kickMsg);
	channel->removeMember(targetClient->getFd());

	if (channel->isEmpty())
		server.removeChannel(channelName);
}
