#include "Server.hpp"
#include "Reply.hpp"

void	cmdInvite(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.size() < 2)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "INVITE"));
		return ;
	}

	std::string targetNick = params[0];
	std::string channelName = params[1];

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

	if (channel->isInviteOnly() && !channel->isOperator(client.getFd()))
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

	if (channel->isMember(targetClient->getFd()))
	{
		client.sendReply(Reply::err_useronchannel(nick, targetNick, channelName));
		return ;
	}

	channel->addInvited(targetClient->getFd());

	client.sendReply(Reply::rpl_inviting(nick, targetNick, channelName));

	std::string inviteMsg = ":" + client.getPrefix() + " INVITE " +
							targetNick + " " + channelName + "\r\n";
	targetClient->sendReply(inviteMsg);
}
