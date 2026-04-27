#include "Server.hpp"
#include "Reply.hpp"

void	cmdPass(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	if (client.isAuthenticated())
	{
		client.sendReply(Reply::err_alreadyregistered(nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "PASS"));
		return ;
	}

	if (params[0] != server.getPassword())
	{
		client.sendReply(Reply::err_passwdmismatch(nick));
		return ;
	}

	client.setAuthenticated(true);
}
