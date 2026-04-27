#ifndef COMMANDDISPATCHER_HPP
# define COMMANDDISPATCHER_HPP

# include <string>
# include <vector>
# include <map>

class Server;
class Client;

typedef void (*CommandHandler)(Server &server, Client &client,
							   std::vector<std::string> &params);

class CommandDispatcher
{
private:
	std::map<std::string, CommandHandler>	_handlers;

	CommandDispatcher(const CommandDispatcher &other);
	CommandDispatcher &operator=(const CommandDispatcher &other);

	static std::string						_toUpper(const std::string &str);

public:
	CommandDispatcher();
	~CommandDispatcher();

	void	registerCommand(const std::string &name, CommandHandler handler);
	void	dispatch(Server &server, Client &client,
					 const std::string &rawMessage);

	static std::vector<std::string>	parseMessage(const std::string &message);
};

void	cmdPass(Server &server, Client &client, std::vector<std::string> &params);
void	cmdNick(Server &server, Client &client, std::vector<std::string> &params);
void	cmdUser(Server &server, Client &client, std::vector<std::string> &params);
void	cmdJoin(Server &server, Client &client, std::vector<std::string> &params);
void	cmdPart(Server &server, Client &client, std::vector<std::string> &params);
void	cmdQuit(Server &server, Client &client, std::vector<std::string> &params);
void	cmdPrivmsg(Server &server, Client &client, std::vector<std::string> &params);
void	cmdKick(Server &server, Client &client, std::vector<std::string> &params);
void	cmdInvite(Server &server, Client &client, std::vector<std::string> &params);
void	cmdTopic(Server &server, Client &client, std::vector<std::string> &params);
void	cmdMode(Server &server, Client &client, std::vector<std::string> &params);

#endif
