#include "CommandDispatcher.hpp"
#include "Client.hpp"
#include "Reply.hpp"

CommandDispatcher::CommandDispatcher()
{
	registerCommand("PASS", cmdPass);
	registerCommand("NICK", cmdNick);
	registerCommand("USER", cmdUser);
	registerCommand("JOIN", cmdJoin);
	registerCommand("PART", cmdPart);
	registerCommand("QUIT", cmdQuit);
	registerCommand("PRIVMSG", cmdPrivmsg);
	registerCommand("KICK", cmdKick);
	registerCommand("INVITE", cmdInvite);
	registerCommand("TOPIC", cmdTopic);
	registerCommand("MODE", cmdMode);
}

CommandDispatcher::~CommandDispatcher()
{
}

void	CommandDispatcher::registerCommand(const std::string &name,
										   CommandHandler handler)
{
	_handlers[name] = handler;
}

std::string	CommandDispatcher::_toUpper(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i)
		result[i] = std::toupper(static_cast<unsigned char>(result[i]));
	return result;
}

std::vector<std::string>	CommandDispatcher::parseMessage(const std::string &message)
{
	std::vector<std::string>	tokens;
	std::string::size_type		i = 0;

	while (i < message.size() && message[i] == ' ')
		++i;

	while (i < message.size())
	{
		if (message[i] == ':' && !tokens.empty()) 
		{
			tokens.push_back(message.substr(i + 1));
			return tokens;
		}

		std::string::size_type start = i;
		while (i < message.size() && message[i] != ' ')
			++i;
		tokens.push_back(message.substr(start, i - start));

		while (i < message.size() && message[i] == ' ')
			++i;
	}
	return tokens;
}

void	CommandDispatcher::dispatch(Server &server, Client &client,
								   const std::string &rawMessage)
{
	std::vector<std::string> tokens = parseMessage(rawMessage);
	if (tokens.empty())
		return ;

	std::string command = _toUpper(tokens[0]);
	tokens.erase(tokens.begin());
	std::map<std::string, CommandHandler>::iterator it = _handlers.find(command);
	if (it != _handlers.end())
	{
		it->second(server, client, tokens); // cmdJoin(server, client, ["#a", "key"])
	}
	else
	{
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		client.sendReply(Reply::err_unknowncommand(nick, command));
	}
}
