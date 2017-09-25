# Namiastka systemu NFS

Realizacja operacji dostępu do zdalnych plików z zachowaniem API systemu UNIX
(open, creat, read, write, lseek, close).

## Kompilacja
Dostarczony jest Makefile
`
make
`
Pliki wyjściowe te client oraz server

## Opis plików
* *client.c* - funkcje naśladujące interfejsy Unixowe open, creat, close, read, write, read, lseek (jedyną różnicą jest dodatkowy błąd ETIMEDOUT w przypadku błędu RPC). Dodatkowo kod przykłądowego klienta wykorzystującego te funkcje.
* *server.c* - implementacja serwera
* *nfs.x* - opis metod RPC i struktur dla rpcgen, z niego wygenerowane są pozostałe pliki źródłowe
