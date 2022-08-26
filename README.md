# Zvířeho větrník
## Turbina a generator
- Ista breeze i-2000
- pri moc silnem vetru se naklopi
- generator 2000 W nominal, 2200 W max
- 3x48 V 40 A
- napeti naprazdno 200 V --> na to nemuzu navrhovat elektroniku, bylo by to
  drahe. V pripade problemu zastavit turbinu.
- 12 polu -> 6 polparu?
  https://www.youtube.com/watch?v=O_9U3-WXe9g

## Zatez
- 2x topna patrona 1200 W 48 V, jde rozdelit
  --> 4x 600 W
- nic jineho, ciste odporova zatez
- patrony nejsou izolovane od vody --> musi byt AC
- https://www.growzone.cz/48V-1200W-topna-patrona-G-6-4-palce-38x-197mm-d8275.htm


## Elektronika
- pulzni rizeni
- mereni vykonu
- musi vydrzet 200 V naprazdno
- 50 A
- asi bude lepsi postupne pripinat jednotliva topna telesa
- 3fazovy jistic s pomocnym kontaktem
- 3fazovy NC stykac pro zkratovani generatoru, napajeni civky pres pomocny
  kontakt jistice
- analogova ochrana proti prepeti musi vypnout stykac (ne MCU, kdyby se
  zasekl, bude naprazdno cca 460V DC)


## Software
- ohrata voda --> odstaveni
- ohrivani vody --> regulace, MPPT
- prilis vysoky vykon / RPM --> odstaveni ?zkrat?
- monitorovani pres MQTT
- Ethernet, ne Wi-Fi

### Vstupy
- teplota vody
- 1x pruchod nulou (frekvence)
- proud a napeti

### Vystupy
- 2x rizeni H-mustku
- 4x spinani topnych teles
- 1x zkratovani 3faz z generatoru

### Regulace prikonu zateze
- strida
- modularni SW, vice rezimu
  - rucni nastaveni uhlu otevreni v %
  - drzeni rucne nastavenych konstantnich RPM turbiny - PID regulator?
  - TODO chytre MPPT - tady urcite PID nepujde, navic se rychle meni rychlost
    vetru


# Odkazy
- https://www.researchgate.net/publication/261429961_A_three-level_MPPT_capability_rectifier_for_high_power_direct_drive_WECS
- https://www.tme.eu/cz/katalog/trifazove-mustky-rizene_112817/


# Datasheets
- https://istabreeze.us/downloadable-documents/


# Poznamky
- napajeni 12V kvuli Vgs FETu
- TODO chladic pro usmernovac
- mereni RMS TODO
- TODO 230V vystup pro cerpadlo
- TODO PCB rivets

- TODO jistic s pomocnym kontaktem
