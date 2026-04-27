#include "Server.hpp"
#include "Reply.hpp"

void	cmdTopic(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "TOPIC"));
		return ;
	}

	std::string channelName = params[0];

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

	if (params.size() == 1)
	{
		if (channel->getTopic().empty())
			client.sendReply(Reply::rpl_notopic(nick, channelName));
		else
			client.sendReply(Reply::rpl_topic(nick, channelName, channel->getTopic()));
		return ;
	}

	if (channel->isTopicLocked() && !channel->isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(nick, channelName));
		return ;
	}

	std::string newTopic = params[1];
	channel->setTopic(newTopic);

	std::string topicMsg = ":" + client.getPrefix() + " TOPIC " +
						   channelName + " :" + newTopic + "\r\n";
	channel->broadcastAll(topicMsg);
}
