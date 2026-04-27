#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief PASS komutunu işler.
 * 
 * Sunucuya bağlanmak için gerekli olan şifreyi kontrol eder. Bu komut,
 * NICK ve USER komutlarından önce gönderilmelidir.
 * 
 * Kullanım: PASS <şifre>
 */
void	cmdPass(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	// Kullanıcı zaten doğrulandıysa tekrar yapamaz
	if (client.isAuthenticated())
	{
		client.sendReply(Reply::err_alreadyregistered(nick));
		return ;
	}

	// Şifre parametresi eksik mi?
	if (params.empty())
	{
		client.sendReply(Reply::err_needmoreparams(nick, "PASS"));
		return ;
	}

	// Gönderilen şifre sunucu şifresiyle eşleşiyor mu?
	if (params[0] != server.getPassword())
	{
		client.sendReply(Reply::err_passwdmismatch(nick));
		return ;
	}

	// Kimlik doğrulaması başarılı
	client.setAuthenticated(true);
}
