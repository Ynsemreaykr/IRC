#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <cerrno>

/**
 * @brief Verilen stringin geçerli bir port numarası (1-65535) olup olmadığını kontrol eder.
 * @param str Port numarası stringi
 * @param port Dönüştürülen port değerinin atanacağı referans
 * @return Geçerliyse true, değilse false
 */
static bool	isValidPort(const char *str, int &port)
{
	char	*endptr;

	errno = 0;
	long val = std::strtol(str, &endptr, 10);
	if (errno != 0 || *endptr != '\0' || val <= 0 || val > 65535)
		return false;
	port = static_cast<int>(val);
	return true;
}

/**
 * @brief Programın giriş noktası.
 * 
 * Argüman kontrolü yapar, sunucu nesnesini oluşturur ve başlatır.
 */
int	main(int argc, char **argv)
{
	// Argüman sayısı kontrolü (program_adı port şifre)
	if (argc != 3)
	{
		std::cerr << "Kullanım: ./ircserv <port> <şifre>" << std::endl;
		return EXIT_FAILURE;
	}

	int port;
	// Port geçerliliği kontrolü
	if (!isValidPort(argv[1], port))
	{
		std::cerr << "Hata: Geçersiz port numarası (1-65535)" << std::endl;
		return EXIT_FAILURE;
	}

	std::string password = argv[2];
	// Şifre boşluk kontrolü
	if (password.empty())
	{
		std::cerr << "Hata: Şifre boş olamaz" << std::endl;
		return EXIT_FAILURE;
	}

	// Sunucuyu oluştur, hazırla ve çalıştır
	Server server(port, password);
	server.init();
	server.run();

	return 0;
}
