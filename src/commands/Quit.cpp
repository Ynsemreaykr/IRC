#include "Server.hpp"

/**
 * @brief QUIT komutunu işler.
 * 
 * Kullanıcının sunucudan ayrılmasını sağlar. Kullanıcının bulunduğu tüm kanallara
 * ayrılma mesajı gönderilir ve kullanıcı sunucudan çıkarılmak üzere işaretlenir.
 * 
 * Kullanım: QUIT [<sebep>]
 */
void	cmdQuit(Server &server, Client &client, std::vector<std::string> &params)
{
	std::string reason = params.empty() ? "İstemci Ayrıldı" : params[0];
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	std::string prefix = client.getPrefix();
	
	// Çıkış mesajını oluştur
	std::string quitMsg = ":" + prefix + " QUIT :" + reason + "\r\n";

	std::map<std::string, Channel> &channels = server.getChannels();
	std::vector<std::string> emptyChannels;

	// Kullanıcının bulunduğu tüm kanallara bildir ve kanallardan çıkar
	for (std::map<std::string, Channel>::iterator it = channels.begin();
		 it != channels.end(); ++it)
	{
		if (it->second.isMember(client.getFd()))
		{
			it->second.broadcast(quitMsg, client.getFd());
			it->second.removeMember(client.getFd());
			// Kanal boş kaldıysa silinmek üzere listeye ekle
			if (it->second.isEmpty())
				emptyChannels.push_back(it->first);
		}
	}

	// Boşalan kanalları temizle
	for (size_t i = 0; i < emptyChannels.size(); ++i)
		server.removeChannel(emptyChannels[i]);

	// İstemciye son hata mesajını gönder ve çıkış için işaretle
	client.sendReply("ERROR :Bağlantı Kesiliyor: " + client.getHostname() +
					 " (Ayrılma: " + reason + ")\r\n");
	client.markForQuit();
}
