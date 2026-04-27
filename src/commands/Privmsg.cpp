#include "Server.hpp"
#include "Reply.hpp"

void	cmdPrivmsg(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_norecipient(nick, "PRIVMSG"));
		return ;
	}

	if (params.size() < 2)
	{
		client.sendReply(Reply::err_notexttosend(nick));
		return ;
	}

	std::string target = params[0];
	std::string message = params[1];

	if (target[0] == '#')
	{
		Channel *channel = server.getChannel(target);
		if (!channel)
		{
			client.sendReply(Reply::err_nosuchchannel(nick, target));
			return ;
		}

		if (!channel->isMember(client.getFd()))
		{
			client.sendReply(Reply::err_cannotsendtochan(nick, target));
			return ;
		}

		std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " +
							  target + " :" + message + "\r\n";
		channel->broadcast(fullMsg, client.getFd());
	}
	else
	{
		Client *targetClient = server.getClientByNick(target);
		if (!targetClient)
		{
			client.sendReply(Reply::err_nosuchnick(nick, target));
			return ;
		}

		std::string fullMsg = ":" + client.getPrefix() + " PRIVMSG " +
							  target + " :" + message + "\r\n";
		targetClient->sendReply(fullMsg);
	}
}
