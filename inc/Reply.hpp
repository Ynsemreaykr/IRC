#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>

# define SERVER_NAME "ircserv"

namespace Reply
{

	inline std::string rpl_welcome(const std::string &nick, const std::string &user,
								   const std::string &host)
	{
		return ":" SERVER_NAME " 001 " + nick +
			   " :Welcome to the IRC Network " + nick + "!" + user + "@" + host + "\r\n";
	}

	inline std::string rpl_yourhost(const std::string &nick)
	{
		return ":" SERVER_NAME " 002 " + nick +
			   " :Your host is " SERVER_NAME ", running version 1.0\r\n";
	}

	inline std::string rpl_created(const std::string &nick)
	{
		return ":" SERVER_NAME " 003 " + nick +
			   " :This server was created today\r\n";
	}

	inline std::string rpl_myinfo(const std::string &nick)
	{
		return ":" SERVER_NAME " 004 " + nick +
			   " " SERVER_NAME " 1.0 o itkol\r\n";
	}

	inline std::string rpl_channelmodeis(const std::string &nick,
										 const std::string &channel,
										 const std::string &modes)
	{
		return ":" SERVER_NAME " 324 " + nick + " " + channel +
			   " +" + modes + "\r\n";
	}

	inline std::string rpl_notopic(const std::string &nick,
								   const std::string &channel)
	{
		return ":" SERVER_NAME " 331 " + nick + " " + channel +
			   " :No topic is set\r\n";
	}

	inline std::string rpl_topic(const std::string &nick,
								 const std::string &channel,
								 const std::string &topic)
	{
		return ":" SERVER_NAME " 332 " + nick + " " + channel +
			   " :" + topic + "\r\n";
	}

	inline std::string rpl_inviting(const std::string &nick,
									const std::string &target,
									const std::string &channel)
	{
		return ":" SERVER_NAME " 341 " + nick + " " + target +
			   " " + channel + "\r\n";
	}

	inline std::string rpl_namreply(const std::string &nick,
									const std::string &channel,
									const std::string &names)
	{
		return ":" SERVER_NAME " 353 " + nick + " = " + channel +
			   " :" + names + "\r\n";
	}

	inline std::string rpl_endofnames(const std::string &nick,
									  const std::string &channel)
	{
		return ":" SERVER_NAME " 366 " + nick + " " + channel +
			   " :End of /NAMES list\r\n";
	}

	/* ─── Error Replies ─── */

	inline std::string err_nosuchnick(const std::string &nick,
									  const std::string &target)
	{
		return ":" SERVER_NAME " 401 " + nick + " " + target +
			   " :No such nick/channel\r\n";
	}

	inline std::string err_nosuchchannel(const std::string &nick,
										 const std::string &channel)
	{
		return ":" SERVER_NAME " 403 " + nick + " " + channel +
			   " :No such channel\r\n";
	}

	inline std::string err_cannotsendtochan(const std::string &nick,
											const std::string &channel)
	{
		return ":" SERVER_NAME " 404 " + nick + " " + channel +
			   " :Cannot send to channel\r\n";
	}

	inline std::string err_norecipient(const std::string &nick,
									   const std::string &command)
	{
		return ":" SERVER_NAME " 411 " + nick +
			   " :No recipient given (" + command + ")\r\n";
	}

	inline std::string err_notexttosend(const std::string &nick)
	{
		return ":" SERVER_NAME " 412 " + nick +
			   " :No text to send\r\n";
	}

	inline std::string err_unknowncommand(const std::string &nick,
										  const std::string &command)
	{
		return ":" SERVER_NAME " 421 " + nick + " " + command +
			   " :Unknown command\r\n";
	}

	inline std::string err_nonicknamegiven(const std::string &nick)
	{
		return ":" SERVER_NAME " 431 " + nick +
			   " :No nickname given\r\n";
	}

	inline std::string err_erroneusnickname(const std::string &nick,
											const std::string &badnick)
	{
		return ":" SERVER_NAME " 432 " + nick + " " + badnick +
			   " :Erroneous nickname\r\n";
	}

	inline std::string err_nicknameinuse(const std::string &nick,
										 const std::string &badnick)
	{
		return ":" SERVER_NAME " 433 " + nick + " " + badnick +
			   " :Nickname is already in use\r\n";
	}

	inline std::string err_usernotinchannel(const std::string &nick,
											const std::string &target,
											const std::string &channel)
	{
		return ":" SERVER_NAME " 441 " + nick + " " + target +
			   " " + channel + " :They aren't on that channel\r\n";
	}

	inline std::string err_notonchannel(const std::string &nick,
										const std::string &channel)
	{
		return ":" SERVER_NAME " 442 " + nick + " " + channel +
			   " :You're not on that channel\r\n";
	}

	inline std::string err_useronchannel(const std::string &nick,
										 const std::string &target,
										 const std::string &channel)
	{
		return ":" SERVER_NAME " 443 " + nick + " " + target +
			   " " + channel + " :is already on channel\r\n";
	}

	inline std::string err_notregistered(const std::string &nick)
	{
		return ":" SERVER_NAME " 451 " + nick +
			   " :You have not registered\r\n";
	}

	inline std::string err_needmoreparams(const std::string &nick,
										  const std::string &command)
	{
		return ":" SERVER_NAME " 461 " + nick + " " + command +
			   " :Not enough parameters\r\n";
	}

	inline std::string err_alreadyregistered(const std::string &nick)
	{
		return ":" SERVER_NAME " 462 " + nick +
			   " :You may not reregister\r\n";
	}

	inline std::string err_passwdmismatch(const std::string &nick)
	{
		return ":" SERVER_NAME " 464 " + nick +
			   " :Password incorrect\r\n";
	}

	inline std::string err_channelisfull(const std::string &nick,
										 const std::string &channel)
	{
		return ":" SERVER_NAME " 471 " + nick + " " + channel +
			   " :Cannot join channel (+l)\r\n";
	}

	inline std::string err_unknownmode(const std::string &nick,
									   char mode)
	{
		std::string m(1, mode);
		return ":" SERVER_NAME " 472 " + nick + " " + m +
			   " :is unknown mode char to me\r\n";
	}

	inline std::string err_inviteonlychan(const std::string &nick,
										  const std::string &channel)
	{
		return ":" SERVER_NAME " 473 " + nick + " " + channel +
			   " :Cannot join channel (+i)\r\n";
	}

	inline std::string err_badchannelkey(const std::string &nick,
										 const std::string &channel)
	{
		return ":" SERVER_NAME " 475 " + nick + " " + channel +
			   " :Cannot join channel (+k)\r\n";
	}

	inline std::string err_chanoprivsneeded(const std::string &nick,
											const std::string &channel)
	{
		return ":" SERVER_NAME " 482 " + nick + " " + channel +
			   " :You're not channel operator\r\n";
	}
}

#endif
