#include "Server.hpp"

void	cmdQuit(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string reason = params.empty() ? "Client Quit" : params[0];
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	std::string prefix = client.getPrefix();
	std::string quitMsg = ":" + prefix + " QUIT :" + reason + "\r\n";

	std::map<std::string, Channel> &channels = server.getChannels();
	std::vector<std::string> emptyChannels;

	for (std::map<std::string, Channel>::iterator it = channels.begin();
		 it != channels.end(); ++it)
	{
		if (it->second.isMember(client.getFd()))
		{
			it->second.broadcast(quitMsg, client.getFd());
			it->second.removeMember(client.getFd());
			if (it->second.isEmpty())
				emptyChannels.push_back(it->first);
		}
	}

	for (size_t i = 0; i < emptyChannels.size(); ++i)
		server.removeChannel(emptyChannels[i]);

	client.sendReply("ERROR :Closing Link: " + client.getHostname() +
					 " (Quit: " + reason + ")\r\n");
	client.markForQuit();
}
