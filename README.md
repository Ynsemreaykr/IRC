# FT_IRC - İnternet Aktarmalı Sohbet Sunucusu (IRC)

## 📌 Proje Hakkında
Bu proje, C++98 standartları kullanılarak geliştirilmiş, tam işlevsel bir IRC (Internet Relay Chat) sunucusudur. Projenin temel amacı, ağ programlama (socket programming), olay döngüsü (event loop) yönetimi ve IRC protokolü standartlarını (RFC 2812) kavramaktır.

Sunucu, birden fazla istemcinin eş zamanlı olarak bağlanmasına, kanallar oluşturmasına, mesajlaşmasına ve kanal yönetimi yapmasına olanak tanır.

## 🚀 Çalıştırma Talimatları

### Derleme
Proje `Makefile` içerir. Derlemek için terminalde şu komutu çalıştırın:
```bash
make
```

### Sunucuyu Başlatma
Sunucu bir port numarası ve bağlantı şifresi ile çalışır:
```bash
./ircserv <port> <şifre>
```
*Örnek:* `./ircserv 6667 sifre123`

### Bağlantı Kurma
Sunucuya herhangi bir IRC istemcisi (Irssi, HexChat, mIRC) veya `nc` (netcat) ile bağlanabilirsiniz:
```bash
nc localhost 6667
```

---

## 🛠️ Teknik Altyapı ve Fonksiyonlar

Proje, düşük seviyeli sistem çağrılarını kullanarak verimli bir G/Ç yönetimi sağlar:

### 1. `poll()` (Olay Döngüsü)
Sunucunun kalbidir. Aynı anda hem yeni bağlantıları dinlemek hem de bağlı olan tüm istemcilerden gelen verileri takip etmek için kullanılır. `select()` fonksiyonuna göre daha modern ve esnektir.
- **İşlevi:** Dosya tanımlayıcılarındaki (file descriptors) olayları (okuma, yazma, hata) izler.

### 2. `socket()`, `bind()`, `listen()` ve `accept()`
- **`socket()`:** İletişim için bir uç nokta (endpoint) oluşturur.
- **`bind()`:** Soketi belirli bir IP ve porta bağlar.
- **`listen()`:** Soketi gelen bağlantı isteklerini kabul edecek şekilde dinleme moduna alır.
- **`accept()`:** Gelen bir bağlantı isteğini kabul eder ve istemci ile iletişim kurmak için yeni bir dosya tanımlayıcı döner.

### 3. `fcntl()` ve `O_NONBLOCK`
Sunucunun donmasını (blocking) önlemek için tüm soketler **non-blocking** (bloklamayan) moda ayarlanır.
- **İşlevi:** `fcntl(fd, F_SETFL, O_NONBLOCK)` komutu ile soketin veri gelmesini beklemeden işlem yapması sağlanır. Bu sayede bir istemci veri gönderirken sunucu diğer istemcilerle ilgilenmeye devam edebilir.

### 4. `recv()` ve `send()`
- **`recv()`:** İstemciden gelen verileri okur.
- **`send()`:** Sunucunun istemciye yanıt veya mesaj iletmesini sağlar.

---

## 💬 Temel Komutlar ve Kullanım

Kullanıcıların sunucuda işlem yapabilmesi için kayıt olmaları gerekir.

### Kayıt ve Giriş
1.  **PASS**: Sunucu şifresini doğrular.
    - *Kullanım:* `PASS <şifre>`
2.  **NICK**: Kullanıcı adı belirler.
    - *Kullanım:* `NICK <takma_ad>`
3.  **USER**: Kullanıcı detaylarını belirler.
    - *Kullanım:* `USER <username> <hostname> <servername> <realname>`

### Kanal İşlemleri
*   **JOIN**: Bir kanala katılır. Eğer kanal yoksa oluşturulur ve katılan ilk kişi **Kanal Operatörü** olur.
    - *Örnek:* `JOIN #yazilim`
*   **PART**: Kanaldan ayrılır.
    - *Örnek:* `PART #yazilim`
*   **PRIVMSG**: Bir kullanıcıya veya kanala mesaj gönderir.
    - *Örnek:* `PRIVMSG #yazilim :Selam millet!`
*   **KICK**: Bir kullanıcıyı kanaldan atar (Sadece operatörler).
    - *Örnek:* `KICK #yazilim Ahmet :Kurallara uy!`
*   **INVITE**: Bir kullanıcıyı davet eder.
    - *Örnek:* `INVITE Mehmet #yazilim`

---

## 👑 Kanal Operatörü ve Modlar

### Operatör Nasıl Olunur?
- Bir kanalı ilk oluşturan kişi otomatik olarak operatör (`@`) olur.
- Mevcut bir operatör, `MODE` komutu ile başka bir kullanıcıya operatörlük verebilir.

### Kanal Modları (`MODE`)
Operatörler kanalı yönetmek için şu modları kullanabilir:
- **`i` (Invite-only):** Sadece davet edilenler girebilir. (`MODE #kanal +i`)
- **`t` (Topic protection):** Sadece operatörler başlığı (topic) değiştirebilir. (`MODE #kanal +t`)
- **`k` (Channel key):** Kanala giriş için şifre koyar. (`MODE #kanal +k <sifre>`)
- **`o` (Operator):** Bir kullanıcıya operatörlük verir veya alır. (`MODE #kanal +o <nick>`)
- **`l` (Limit):** Kanala girebilecek maksimum kullanıcı sayısını belirler. (`MODE #kanal +l 10`)

---

## 📂 Proje Yapısı
- `src/`: Kaynak kodlar (`.cpp` dosyaları).
- `inc/`: Başlık dosyaları (`.hpp` dosyaları).
- `src/commands/`: Her bir IRC komutunun (JOIN, NICK, vb.) ayrı ayrı implementasyonları.
- `Makefile`: Derleme kuralları.

---
*Bu proje 42 okulu kapsamında eğitim amaçlı geliştirilmiştir.*
