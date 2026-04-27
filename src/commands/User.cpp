#include "Server.hpp"
#include "Reply.hpp"

/**
 * @brief USER komutunu işler.
 * 
 * İstemcinin kullanıcı adını ve diğer bilgilerini belirler. Kayıt sürecinin
 * bir parçasıdır. NICK ve PASS komutlarıyla birlikte kullanıldığında kayıt tamamlanır.
 * 
 * Kullanım: USER <kullanıcı_adı> <mod> <kullanılmayan> :<gerçek_ad>
 */
void	cmdUser(Server &server, Client &client, std::vector<std::string> &params)
{
	(void)server;
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	// PASS komutu ile şifre doğrulanmadıysa işlem yapma
	if (!client.isAuthenticated())
	{
		client.sendReply(Reply::err_notregistered(nick));
		return ;
	}

	// Kullanıcı bilgileri zaten ayarlanmışsa tekrar yapılamaz
	if (client.isUserSet())
	{
		client.sendReply(Reply::err_alreadyregistered(nick));
		return ;
	}

	// Parametre sayısı kontrolü (en az 4 parametre gereklidir)
	if (params.size() < 4)
	{
		client.sendReply(Reply::err_needmoreparams(nick, "USER"));
		return ;
	}

	std::string username = params[0];

	bool wasRegistered = client.isRegistered();

	// Kullanıcı bilgilerini güncelle
	client.setUsername(username);
	client.setUserSet(true);

	// Eğer bu komutla kayıt tamamlandıysa (NICK de ayarlanmışsa) hoşgeldin mesajlarını gönder
	if (!wasRegistered && client.isRegistered())
	{
		client.sendReply(Reply::rpl_welcome(client.getNickname(),
			client.getUsername(), client.getHostname()));
		client.sendReply(Reply::rpl_yourhost(client.getNickname()));
		client.sendReply(Reply::rpl_created(client.getNickname()));
		client.sendReply(Reply::rpl_myinfo(client.getNickname()));
	}
}
