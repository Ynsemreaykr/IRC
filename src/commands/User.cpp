#include "Server.hpp"
#include "Reply.hpp"

void	cmdUser(Server &server, Client &client, std::vector<std::string> &params)
{
	(void)server;
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	if (!client.isAuthenticated())
	{
		client.sendReply(Reply::err_notregistered(nick));
		return ;
	}

	if (client.isUserSet())
	{
		client.sendReply(Reply::err_alreadyregistered(nick));
		return ;
	}

	if (params.size() < 4)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "USER"));
		return ;
	}

	std::string username = params[0];

	bool wasRegistered = client.isRegistered();

	client.setUsername(username);
	client.setUserSet(true);

	if (!wasRegistered && client.isRegistered())
	{
		client.sendReply(Reply::rpl_welcome(client.getNickname(),
			client.getUsername(), client.getHostname()));
		client.sendReply(Reply::rpl_yourhost(client.getNickname()));
		client.sendReply(Reply::rpl_created(client.getNickname()));
		client.sendReply(Reply::rpl_myinfo(client.getNickname()));
	}
}
