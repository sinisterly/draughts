# draughts Qt
**1. Kompilacja**  
Serwer:  g++ -Wall -pthread -std=c++11 -o server server.cpp  
Klient: przy użyciu QtCreator lub z linii poleceń: qmake && make

**2. Protokół komunikacyjny**  
Komunikacja odbywa się poprzez TCP/IP na porcie 1050. Każda wiadomość wysłana od klienta do serwera ma 20 znaków, natomiast każda wiadomość wysłana od serwera do klienta zakończona jest znakiem nowej linii. Klient po podłączeniu do serwera wysyła numer stołu do którego chce dołączyć oraz swój nick. Serwer następnie przydziela klienta do danego stołu oraz tworzy 2 wątki – jeden czytający informacje od tego klienta, drugi wysyłający do niego.

**3. Implementacja**  
Klient zaimplementowany został w Qt przy użyciu frameworka Graphics View. Plansza jest obiektem typu QGraphicsScene, który agreguje obiekty QgraphicsItem(pola oraz pionki). 
Komunikacja odbywa się w klasie Connection, która posiada obiekt QtcpSocket.
Serwer posiada 200 stołów do gry: 0-199. Każdy stół posiada wirtualnego gracza, który przechowuje ruchy wykonane w trakcie trwającej gry, dzięki temu obserwator może dołączyć do trwającej gry i ją oglądać na bieżąco.


