## Licznik rowerowy

### Funkcje:
- pomiar prędkości z wykorzystaniem odbiornika GPS,
- pomiar kadencji (częstotliwość pedałowania) za pomocą czujnika halla,
- obliczanie średniej prędkości, średniej kadencji, maksymalnej prędkości itd.
- wykrywanie wzniosów i spadków za pomocą GPS,
- kierunek poruszania wyznaczany za pomocą magnetometru (w odbiorniku GPS),
- zapisywanie danych z odbiornika GPS w postaci plików .gpx (lub inny) na kartę microSD, które można zaimportować do aplikacji,
- może wyświetlanie przybliżonej lokalizacji,
- wyświetlanie na ekranie kierunku, aktualnej prędkości, aktualnej kadencji, średniej prędkości.

### Hardware:
- Arduino Uno,
- odbiornik u-blox NEO-6M (GPS + magnetometr),
- czujnik Halla A3144,
- wyświetlacz LCD 2.8" SPI XPT2046.
 
### Software:
- biblioteki:
    - TinyGPS++ - parsowanie NMEA,
    - HMC5883L_Simple - do kompasu,
    - ?? - do obsługi LCD,
    - ?? - do obsługi czujnika Halla.
- IDE: VSCode i PlatformIO

test