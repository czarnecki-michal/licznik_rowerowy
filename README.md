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
- Arduino Uno / ESP32 / Arduino Mega,
- odbiornik u-blox NEO-6M (GPS + magnetometr),
- czujnik Halla A3144,
- wyświetlacz LCD 2.8" SPI ILI9341 z wejściem na kartę SD.
 
### Software:
- biblioteki:
    - TinyGPS++ - parsowanie NMEA.
    - HMC5883L_Simple - do kompasu.
    - ILI9341_due, SPI - do obsługi LCD.
    - I2C - do obsługi czujnika Halla.
    - SD, SPI - do obsługi karty SD.
- IDE: VSCode i PlatformIO

### Postęp prac:
#### Zrobiono:
- ~~pomiar prędkości z wykorzystaniem odbiornika GPS,~~
- ~~wyświetlanie parametrów na wyświetlaczu,~~
- ~~pomiar kadencji (częstotliwość pedałowania) za pomocą czujnika halla,~~
- ~~obliczanie średniej prędkości, średniej kadencji, maksymalnej prędkości itd.~~
- ~~kierunek poruszania wyznaczany za pomocą magnetometru (w odbiorniku GPS),~~
- ~~zapisywanie danych z odbiornika GPS w postaci plików .gpx (lub inny) na kartę microSD, które można zaimportować do aplikacji,~~
- ~~wyświetlanie na ekranie kierunku, aktualnej prędkości, aktualnej kadencji, średniej prędkości.~~
- ~~przygotowanie prototypu na płytce stykowej.~~
#### Do zrobienia:
- uporządkowanie kodu, poprawki.
- obsługa dotyku lub przycisków.
- rozpoczynanie, zatrzymywanie i kończenie aktywności.
- powiadomienie o rozpoczęciu, zatrzymaniu, zakończeniu aktywności.
- przeniesienie projektu na płytkę uniwersalną.
- przeniesienie projektu na Arduino Mega/ESP32.

