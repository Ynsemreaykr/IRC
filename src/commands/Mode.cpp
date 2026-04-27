#include "Server.hpp"
#include "Client.hpp"
#include "Reply.hpp"
#include <sstream>

static void	handleModeI(Channel &channel, Client &client, bool enable,
						std::string &applied, std::string &appliedParams)
{
	(void)appliedParams;
	if (!channel.isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(client.getNickname(),
			channel.getName()));
		return ;
	}
	channel.setInviteOnly(enable);
	applied += (enable ? "+i" : "-i");
}

static void	handleModeT(Channel &channel, Client &client, bool enable,
						std::string &applied, std::string &appliedParams)
{
	(void)appliedParams;
	if (!channel.isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(client.getNickname(),
			channel.getName()));
		return ;
	}
	channel.setTopicLock(enable);
	applied += (enable ? "+t" : "-t");
}

static void	handleModeK(Channel &channel, Client &client, bool enable,
						std::vector<std::string> &params, size_t &paramIdx,
						std::string &applied, std::string &appliedParams)
{
	if (!channel.isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(client.getNickname(),
			channel.getName()));
		return ;
	}
	if (enable)
	{
		if (paramIdx >= params.size())
		{
			client.sendReply(Reply::err_needmoreparams(client.getNickname(), "MODE"));
			return ;
		}
		std::string key = params[paramIdx++];
		channel.setKey(key);
		applied += "+k";
		appliedParams += " " + key;
	}
	else
	{
		channel.setKey("");
		applied += "-k";
	}
}

static void	handleModeO(Server &server, Channel &channel, Client &client,
						bool enable, std::vector<std::string> &params,
						size_t &paramIdx, std::string &applied,
						std::string &appliedParams)
{
	if (!channel.isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(client.getNickname(),
			channel.getName()));
		return ;
	}
	if (paramIdx >= params.size())
	{
		client.sendReply(Reply::err_needmoreparams(client.getNickname(), "MODE"));
		return ;
	}
	std::string targetNick = params[paramIdx++];
	Client *target = server.getClientByNick(targetNick);
	if (!target)
	{
		client.sendReply(Reply::err_nosuchnick(client.getNickname(), targetNick));
		return ;
	}
	if (!channel.isMember(target->getFd()))
	{
		client.sendReply(Reply::err_usernotinchannel(client.getNickname(),
			targetNick, channel.getName()));
		return ;
	}
	if (enable)
		channel.addOperator(target->getFd());
	else
		channel.removeOperator(target->getFd());
	applied += (enable ? "+o" : "-o");
	appliedParams += " " + targetNick;
}

static void	handleModeL(Channel &channel, Client &client, bool enable,
						std::vector<std::string> &params, size_t &paramIdx,
						std::string &applied, std::string &appliedParams)
{
	if (!channel.isOperator(client.getFd()))
	{
		client.sendReply(Reply::err_chanoprivsneeded(client.getNickname(),
			channel.getName()));
		return ;
	}
	if (enable)
	{
		if (paramIdx >= params.size())
		{
			client.sendReply(Reply::err_needmoreparams(client.getNickname(), "MODE"));
			return ;
		}
		std::string limitStr = params[paramIdx++];
		char *endptr;
		long limit = std::strtol(limitStr.c_str(), &endptr, 10);
		if (*endptr != '\0' || limit <= 0)
		{
			client.sendReply(Reply::err_needmoreparams(client.getNickname(), "MODE"));
			return ;
		}
		channel.setUserLimit(static_cast<int>(limit));
		applied += "+l";
		appliedParams += " " + limitStr;
	}
	else
	{
		channel.setUserLimit(0);
		applied += "-l";
	}
}

static std::string	collapseModeString(const std::string &modes)
{
	std::string result;
	char currentSign = 0;

	for (size_t i = 0; i < modes.size(); ++i)
	{
		if (modes[i] == '+' || modes[i] == '-')
		{
			if (modes[i] != currentSign)
			{
				currentSign = modes[i];
				result += currentSign;
			}
		}
		else
		{
			result += modes[i];
		}
	}
	return result;
}

void	cmdMode(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname();

	if (!client.isRegistered())
	{
		client.sendReply(Reply::err_notregistered(nick.empty() ? "*" : nick));
		return ;
	}

	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "MODE"));
		return ;
	}

	std::string target = params[0];

	if (target[0] != '#')
		return ;

	Channel *channel = server.getChannel(target);
	if (!channel)
	{
		client.sendReply(Reply::err_nosuchchannel(nick, target));
		return ;
	}

	if (params.size() == 1)
	{
		std::string modes = channel->getModeString();
		std::string modeParams;
		if (!channel->getKey().empty())
			modeParams += " " + channel->getKey();
		if (channel->getUserLimit() > 0)
		{
			std::ostringstream oss;
			oss << channel->getUserLimit();
			modeParams += " " + oss.str();
		}
		client.sendReply(Reply::rpl_channelmodeis(nick, target,
			modes + modeParams));
		return ;
	}

	if (!channel->isMember(client.getFd()))
	{
		client.sendReply(Reply::err_notonchannel(nick, target));
		return ;
	}

	std::string modeStr = params[1];
	std::vector<std::string> modeParams(params.begin() + 2, params.end());
	size_t paramIdx = 0;

	bool enable = true;
	std::string applied;
	std::string appliedParams;

	for (size_t i = 0; i < modeStr.size(); ++i)
	{
		char c = modeStr[i];

		if (c == '+')
		{
			enable = true;
			continue ;
		}
		if (c == '-')
		{
			enable = false;
			continue ;
		}

		switch (c)
		{
			case 'i':
				handleModeI(*channel, client, enable, applied, appliedParams);
				break ;
			case 't':
				handleModeT(*channel, client, enable, applied, appliedParams);
				break ;
			case 'k':
				handleModeK(*channel, client, enable, modeParams, paramIdx,
							applied, appliedParams);
				break ;
			case 'o':
				handleModeO(server, *channel, client, enable, modeParams,
							paramIdx, applied, appliedParams);
				break ;
			case 'l':
				handleModeL(*channel, client, enable, modeParams, paramIdx,
							applied, appliedParams);
				break ;
			default:
				client.sendReply(Reply::err_unknownmode(nick, c));
				break ;
		}
	}

	if (!applied.empty())
	{
		std::string modeMsg = ":" + client.getPrefix() + " MODE " + target +
							  " " + collapseModeString(applied) + appliedParams + "\r\n";
		channel->broadcastAll(modeMsg);
	}
}
