#include "Server.hpp"
#include "Reply.hpp"

static bool	isValidNick(const std::string &nick)
{
	if (nick.empty() || nick.size() > 9)
		return false;
	if (!std::isalpha(static_cast<unsigned char>(nick[0])) && nick[0] != '_')
		return false;
	for (size_t i = 1; i < nick.size(); ++i)
	{
		char c = nick[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) &&
			c != '-' && c != '_' && c != '[' && c != ']' &&
			c != '{' && c != '}' && c != '\\' && c != '|')
			return false;
	}
	return true;
}

void	cmdNick(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string currentNick = client.getNickname().empty() ? "*" : client.getNickname();

	if (!client.isAuthenticated())
	{
		client.sendReply(Reply::err_notregistered(currentNick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_nonicknamegiven(currentNick));
		return ;
	}

	std::string newNick = params[0];

	if (!isValidNick(newNick))
	{
		client.sendReply(Reply::err_erroneusnickname(currentNick, newNick));
		return ;
	}

	Client *existing = server.getClientByNick(newNick);
	if (existing && existing->getFd() != client.getFd())
	{
		client.sendReply(Reply::err_nicknameinuse(currentNick, newNick));
		return ;
	}

	std::string oldPrefix = client.getPrefix();
	bool wasRegistered = client.isRegistered();

	client.setNickname(newNick);
	client.setNickSet(true);

	if (wasRegistered)
	{
		std::string msg = ":" + oldPrefix + " NICK " + newNick + "\r\n";
		std::set<int> notified;
		std::map<std::string, Channel> &channels = server.getChannels();
		for (std::map<std::string, Channel>::iterator it = channels.begin();
			 it != channels.end(); ++it)
		{
			if (it->second.isMember(client.getFd()))
			{
				const std::set<int> &members = it->second.getMembers();
				for (std::set<int>::const_iterator m = members.begin();
					 m != members.end(); ++m)
				{
					if (notified.find(*m) == notified.end())
					{
						Client *c = server.getClientByFd(*m);
						if (c)
							c->sendReply(msg);
						notified.insert(*m);
					}
				}
			}
		}
		if (notified.find(client.getFd()) == notified.end())
			client.sendReply(msg);
	}

	if (!wasRegistered && client.isRegistered())
	{
		client.sendReply(Reply::rpl_welcome(client.getNickname(),
			client.getUsername(), client.getHostname()));
		client.sendReply(Reply::rpl_yourhost(client.getNickname()));
		client.sendReply(Reply::rpl_created(client.getNickname()));
		client.sendReply(Reply::rpl_myinfo(client.getNickname()));
	}
}
