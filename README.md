# Traceroute

Prosta implementacja programu traceroute, które znajduje trasę do zadanego adresu IP.
Projekt zrealizowany w ramach przedmiotu Sieci Komputerowe na Uniwersytecie Wrocławskim w kwietniu 2021.
Autor: Marcin Bieganek.

### Działanie

Program w pętli wysyła zapytania echo ICMP ze zwiększanym ttl (zaczynając od 1). Na podstawie odpowiedzi ICMP time exceeded lub echo reply wypisuje on na standardowe wyjście informacje o kolejnym kroku wraz z średnim czasem odpowiedzi.

### Obsługa

Projekt zawiera Makfile. Wykonując polecenie `make` możemy skompilować program.
Polecenie `make clean` czyści katalog z plików pośrednich. 
Polecenie `make distclean` czyści katalog z plików pośrednich oraz pliku wykonywalnego.

Po kompilacji program można uruchomić poleceniem `./traceroute <ip_addr>`.
Na przykład:

`./traceroute 8.8.8.8`
