**Gerät mit GPS pps Blitz\
für das Setzen von optischen Zeitstempeln\
in astronomischen Videos von Digitalkameras**

Björn Kattentidt - IOTA/ES - bjoern@kattentidt-astro.de

**Motivation:**

Bisherige Analogkameras konnten mit VTI (video time inserter) im
Videodatenstrom mit Zeitinformationen gestempelt werden. Moderne
Digitalkameras ermöglichen dies nur mit aufwändiger s. Daraus ergaben
sich viele auch sehr gute Kamerasysteme im Eigenbau mit erheblichem
Aufwand. Mit modernsten „smart telescopes" wird das Problem deutlich,
daß es noch schwerer möglich sein wird die speziellen Wünsche nach
Zeitmarken mit Genauigkeit im Bereich von Millisekunden in Videos zu
realisieren.

Ein Gerät wird interessant sein, welches man an jede beliebige Optik
anbringt und beim Anschalten nach wenigen Minuten NMEA-Daten verwendet
um die Anforderungen für Auswertung von Zeitmarken in
Sternbedeckungsvideos zu erfüllen. Ein solches ist nun verfügbar.

**Eigenschaften im Überblick:**

- pps Blitz (40 ms) mittels LED an Klammer mit Leitung (ca. 1 m) zum
  Steuergerät

- Helligkeit über Drehpotentiometer mit integriertem Schalter
  einstellbar

- Zeitliche Blitzdistanz in weiten Bereichen mit zwei BCD Kodierrädern
  einstellbar

- Diverse Modi voreingestellt am oberen Zahlenbereich, bis 100 Modi
  möglich

- Sonderfunktionen: Keine Einstellung notwendig, GPS wird für GPS
  synchrone Blitzmuster verwendet. Mit ungültigem GPS-Signal wechselt
  die interne Arduino LED im Sekundentakt und die LED an der Klammer
  blitzt dann jede Sekunde den pps.

- 3,2 V LiFePo4 Akku mit 600 mAh, Verbrauch: typisch 80 mA

- Betriebsdauer ca. 7 Stunden

- Letzte 0,5 ... 1 Stunde „low batt." Anzeige durch schnell blinkende
  Status-LED

- Tiefentladeschutz: Gerät schaltet ab, Reststrom 2mA, alle LED aus

- 2h Laden über USB-C, Ladegerät integriert.\
  Unteres Fenster: Rotes Licht „Laden", oranges Licht "Voll"

- Arduino NANO mit open source Kode über DC-DC Wandler (3,2 V zu 5,0 V)

- GPS Modul ATGM336H (3,2 V) GPS+BD\
  Warmstart 10-25 Sek. (Autor), Kaltstart 35 Sek. (Hersteller),\
  bis zu einigen Minuten bei ungünstigem Empfang\
  Mittleres Fenster: pps GPS Bereitschaft durch invertiert blinkende LED

- Schritt für Schritt Anleitung zum Selbstbau \[1\]

- Zweitlösung für Zeitmarken in Bedeckungsvideos, wenn das primäre
  Zeitsystem aus technischen Gründen plötzlich ausfällt

![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image1.jpeg){width="1.89375in"
height="1.89375in"}
![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image2.jpeg){width="1.882638888888889in"
height="1.882638888888889in"}
![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image3.jpeg){width="1.882638888888889in"
height="1.882638888888889in"}

Abbildung 1: GPS pps Blitzer, fertiges Gerät

**Funktionalität:**

Eine blitzende LED ist an einer Klammer angebracht. Diese ist mit einer
Leitung mit dem Steuergerät verbunden um die mechanische Last an der
Optik zu verringern und die Bedienung wackelfrei zu ermöglichen.

Die Klammer wird an die Taukappe der Optik mit LED nach innen in den
Strahlengang angeklemmt. Bei großem Abstand zum Rand des Strahlengangs
kann die Klammerweite modifiziert werden indem der schwarze Zylinder
verlängert wird.

Der Code ist derart optimiert, daß die Blitze weniger als 100 µs nach
dem pps Signal (peak per second) des GPS erscheinen. Die loop-Zeit
beträgt unter 0,25 ms. Das Lichtsignal ist demnach deutlich weniger als
1 ms verzögert. Eigene Messungen mithilfe einer DVTI ergaben maximal
2ms. [Dies ist aber derzeit noch nicht zuverlässig nachgemessen worden
und muß noch mittels EXTA festgestellt werden!]{.mark}

Der Blitz hat außer bei #00 immer dieselbe Dauer von 40ms.

Im Modus #00 leuchtet die LED lang. So kann die Helligkeit mit dem
Schaltpotentiometer eingestellt werden. Am besten sieht man gerade noch
ein schwaches Aufleuchten im Video. PyOTE kann sogar Markierungen
auswerten, wenn man das Blitzen nicht mehr sieht.

Die speziellen Funktionen (#87-#99) verwenden dagegen NMEA-Daten, auch
für die Sekunde, während die zeitliche Triggerung über das deutlich
genauere pps-Signal verwendet wird.\
Erst mit gültigen NMEA-Daten arbeiten die Sonderfunktionen wie erwartet
und die obere obere Status-LED am Arduino NANO verlischt! Bei ungültigen
oder keinen NMEA-Daten blinkt die obere Status-LED langsam (wechselt
jede Sekunde).

Bei vollem Akku wird das Gerät ca. 7 Stunden blitzen wobei die letzte
halbe bis eine Stunde die Status-LED schnell blinkt. Dann versetzt der
Tiefentladeschutz das Gerät in den Ruhemodus mit 2 mA Reststrom. Das
Gerät muß zum Schutz des Akkus dennoch mit dem Schalter vollständig
abgeschaltet werden! Der Akku hat 600 mAh Kapazität bei max. 80 mA
Verbrauch. Der Akku wird über den USB-C Anschluß geladen. Ein Ladegerät
ist dazu integriert. Das Steuergerät kann im ausgeschalten Zustand oder
auch während des Betriebs über die USB-Buchse geladen werden. Dennoch
ist es besser während des Betriebs nicht zu laden zum Schutz des
GPS-Moduls, welches dann mit dem Akku verbunden ist. Die Ladezeit
beträgt ca. zwei Stunden.

Ein Tiefentladeschutz ist implementiert. Bei geringer Spannung (3,1 V)
blinkt die obere Status-LED in schneller Folge (4 Hertz). Bei
Entladeschlußspannung (2,8 V) schaltet sich das Gerät in den Ruhemodus.
Ist der Tiefentladeschutz aktiviert, gehen alle LED aus. Es fließt aber
noch ein Reststrom wodurch der Akku nach weiteren 2 Tagen tiefentladen
wird. [Daher muß das Gerät immer per Schalter ausgeschaltet werden, denn
eine einzige Tiefentladung kann den LiFePo4 Akku
zerstören.]{.underline}\
Das Gerät kann reaktiviert werden, indem man es am Drehknopf
ausschaltet, und wieder anschaltet. Geht es sofort wieder aus mit dem
Anschalten ein paar Sekunden warten.

**Verwendungsablauf:**

Gerät an der Öse aufhängen oder mit Öse zum Zenit aufstellen, dann hat
das GPS optimalen Empfang.

Die Klammer an der Taukappe anklemmen mit der LED im Strahlengang in die
Optik zeigend.

Mit Drehknopf nach rechts einschalten.

Zunächst ist die 1pps-LED dauerhaft an und die Status-LED blinkt im
Sekundenrhythmus, weil noch kein pps-Signal und keine NMEA Daten erkannt
wurden. Wenn die Status-LED aus ist und das 1pps-Signal im
Sekundenrhythmus kurz aus geht, ist das Gerät betriebsbereit.\
Dies wird unter freiem Himmel binnen einer Minute geschehen, manchmal
dauert es mehrere Minuten.

Blinkt die Status-LED langsam, liegen noch keine gültigen NMEA Daten
vor. Damit können die speziellen Funktionen von #99 abwärts bis #82
nicht verwendet werden. Allerdings kann das exakte 1pps-Signal mit den
Funktionen #01 bis #81 verwendet werden, wenn schon die 1pps-LED blinkt.
Blinkt die Status-LED schnell, bedeutet dies, daß der Akku bald leer
sein wird und das Gerät dann in den Ruhemodus ausschaltet.

Für die LED an der Klammer wird das exakte elektrische pps-Signal des
GPS-Moduls verwendet.

Die Kodierräder zunächst auf #00 stellen und die benötigte Helligkeit am
Potentiometer einstellen.

Dann den gewünschten Modus einstellen. Man kann jederzeit in jeden
anderen Modus mit den beiden Kodierrädern schalten. Nach einer Sekunde
ist das Signal dann korrekt.\
Nun kann das Gerät benutzt werden um Millisekunden genaue Zeitstempel
optisch zu setzen.

[Nach der Verwendung das Gerät am Drehknopf wieder
abschalten!]{.underline}

**Funktionsweise:**

An der Spitze der Klammer blitzt im Sekundentakt eine rote LED GPS
synchron kurz auf. Wird die Zahl zum Beispiel auf #11 gestellt blitzt
die LED jede elfte Sekunde. So kann jede zyklische Wiederholung
eingestellt werden und der Beginn mit etwas Übung auch mit Hilfe einer
sehr genauen Uhr voreingestellt werden, indem man die Zahl beginnend bei
#01 geschwind höher stellt.

Zum Verständnis: Ein interner Zähler zählt nach einem Blitzen von Null
an aufwärts bis er die eingestellte Zahl erreicht hat. Stellt man diese
Zahl nach einem Blitzen zu bestimmter Sekunde höher, hat man die Sequenz
auf den Beginn eingestellt.\
Bis die eingestellte Zahl vom Zähler überschritten wird, kann man weiter
die Kodierräder einstellen. Mit etwas Geschick und Überlegung, etwa den
Zehner zuerst zu stellen, bleibt ausreichend Zeit mit den Einstellungen
fertig zu werden.

Dies ergibt zwar viele Freiheiten, aber diese Art der Einstellung
erfordert Geschick und man benötigt noch eine genaue Zeit (Android: z.B.
AtomUhr App). Startet und stoppt man das Video mit einer Roboter App
(z.B. MacroDroid) bei Verwendung eines „smart telescope" mit
Mobiltelefon, dann sind Beginn und Ende des Videos sekundengenau, was
die Feststellung der Minute wesentlich erleichtert.

Zur deutlich leichteren Nutzung ohne weitere Apps sind Sonderfunktionen
von #99 herab implementiert (siehe Liste). Diese Modi nutzen NMEA-Daten
und der Blitz leuchtet daher immer genau zur eingestellten Sequenz
entsprechend der GPS Sekunde auf!

Die Sonderfunktionen als auch längere Blitzabstände sind nur geeignet
für Kameras bei denen auf Zeit Synchronität Verlaß ist wie etwa bei ASI
Kameras, während sie weniger geeignet sind für QHY Kameras, welche eine
Firmware zu haben scheinen, wodurch die echten Belichtungszeiten
fluktuieren sowie unvorhersehbare Pausen erscheinen (eigene Erfahrung
des Autors). Bei Belichtungszeiten über 0,1 Sekunden ist das allerdings
wenig relevant für genaue Zeitstempel.

Hat man es eilig, muß man eine manuell einstellbare Blitzfolge verwenden
und den Beginn selbst passend mit einer genauen Vergleichsuhr
synchronisieren (Android: z.B. AtomUhr APP).

**Liste der verfügbaren Modi (erweiterbar)**

Achtung: Der zum Video eingestellte Modus sollte immer notiert werden!

#00: Nur zu Zwecken der Helligkeitseinstellung der LED an der Klammer.\
Die LED leuchtet 0,8 Sekunden lang nach jedem pps.

#01 ... #81: Die LED blitzt im Abstand der eingestellten Sekunden.
Dazwischen bleibt sie aus. Eine gezielte Einstellung kann erreicht
werden gemäß Beschreibung im Abschnitt Verwendung.

+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| BCD               | Blitze bei genannter GPS-Sekunde          | LUT               | (Kode)            |                   |
+:=================:+:=========================================:+:=================:+:=================:+:=================:+
| Sekundenmaske vollständig synchron                                                                                        |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #99               | 00, 05, 10, 15, 20, 25, 30, 35, 40, 45,   | LUT_05s           | FF                |                   |
|                   | 50, 55                                    |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #98               | 00, 10, 20, 30, 40, 50                    | LUT_10s           | FE                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #97               | 00, 15, 30, 45                            | LUT_15s           | FD                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #96               | 00, 20, 40                                | LUT_20s           | FC                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #95               | 00, 30                                    | LUT_30s           | FB                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #94               | 00                                        |                   | FA                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| Sekundenmaske mit Auslassung am Nulldurchgang (Minutenwechsel)                                                            |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #93               | 05, 10, 15, 20, 25, 30, 35, 40, 45, 50,   | LUT_05s0          | EF                |                   |
|                   | 55                                        |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #92               | 10, 20, 30, 40, 50                        | LUT_10s0          | EE                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #91               | 15, 30, 45                                | LUT_15s0          | ED                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #90               | 20, 40                                    | LUT_20s0          | EC                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #89               | 30                                        | LUT_30s0          | EB                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| Sekundenmaske zeitlich kodiert                                                                                            |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #88               | 00, 05, 10, 20, 40                        | LUT_DF            | DF                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #87               | 00, 11, 23, 34, 45 (ähnlich Pseudo-Barker | LUT_DE            | DE                |                   |
|                   | Kode)                                     |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #86               | 00, 11, 23, 34, 45; Minute binär: 48-53   | LUT_DD            | DD                |                   |
|                   | und 54-59, LSB beginnend, Paritätsbit 46  |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| Sekundenmaske ohne Vorlaufereignis vor dem Minutenwechsel                                                                 |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #85               | 00, 05, 10, 15, 20, 25, 30, 35, 40, 45,   | LUT_05sv0         | CF                |                   |
|                   | 50                                        |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #84               | 00, 10, 20, 30, 40                        | LUT_10sv0         | CE                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #83               | 00, 15, 30                                | LUT_15sv0         | CD                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #82               | 00, 20                                    | LUT_20sv0         | CC                |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
|                                                                                                                           |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+
| #01 ... #81       | Sekunden Abstand, beginnend mit           |                   |                   |                   |
|                   | beliebiger Sekunde                        |                   |                   |                   |
+-------------------+-------------------------------------------+-------------------+-------------------+-------------------+

Die Sequenzen #94 bis #99 ermöglichen GPS-synchronisierte Blitze in
äquidistantem Zeitabstand.

Die Sequenzen #89 bis #93 haben keinen Blitz bei GPS Sekunde 0 zur
Einordnung der Blitze relativ zur Minute.

Die Sequenzen #88 und #87 erlauben es mit Wissen über die Minute des
Beginns oder Ende des Videos zuverlässige Zeitangaben mit PyMovie/PyOTE
zu ermitteln und zu prüfen.

Die Sequenz #86 ist gleich wie #87 aber mit zusätzlich integrierter
Kodierung der Minute mit Wiederholung und Paritätsbit an fixen
Positionen, ähnlich der Methode beim DCF77 Signal. Damit ist es nicht
mehr nötig die Minute zu notieren.

Beispiel:\
1 0 0 0 1 1 binär = 35 dezimal: 35-te Minute, LSB kommt zuerst\
Anzahl 1-Bits ungerade, Parität = 1:\
Gesamtzahl der Einsen mit Parität gerade, das heißt gerade Parität\
Das Paritätsbit wird bei gerader Parität so berechnet, dass die
Gesamtanzahl der Einsen gerade ist.

![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image4.png){width="6.297916666666667in"
height="1.1805555555555556in"}Abbildung 2: Modus #86, Minute 35

Die Sequenzen #82 bis #85 haben diesen fehlenden Blitz an letzter Stelle
in der Minutensequenz. Damit läßt sich der Minutenbeginn leichter
feststellen.

Vorteil der Sonderfunktionen liegt in der geringeren Störung der
Lichtkurve durch die Blitze im Gegensatz zu einem Blitz in jeder
Sekunde.\
Jegliche anderen Kodierungen können implementiert werden.

Nachteil der Sonderfunktionen ist, daß der Empfang der NMEA-Daten gültig
sein muß, was länger dauern kann oder auch bei schlechtem Empfang
Unterbrechungen erfahren kann.

Benutzt man nur das pps-Signal in den Modi #01 bis #81 ist es
ausreichend wenn die LED des GPS blinkt.

**Vier Anwendungsfälle:**

#01:\
LED blitzt jede Sekunde. Grundfunktion. Verwendung wenn sehr schnell ein
genaues Sekundensignal notwendig wird oder Bilder bei hohen Bildraten
verloren gehen oder wenn die Bildzeiten der Kamera jittern. Es müssen
Minutenwechsel und Minute anders ermittelt werden, z.B. durch Notieren
der Minute sowie Beginn des Videos per Automatismus zu bestimmter
GPS-Sekunde.

#85:\
LED blitzt alle 5 Sekunden außer in der 55-ten Sekunde. Bei gut
erkennbaren Blitzen ist der Minutenwechsel gut auswertbar. Schnelle
Ersatzlösung bei Versagen des Primärsystems. Minute muß notiert werden.

#88:\
LED blitzt bei 00, 05, 10, 20 und 40 Sekunden. Mit wenig Blitzen hat man
bei mehr als 2 Minuten langen Videos eine sichere Erkennung des
Minutenwechsels. Minute muß notiert werden.

#86\
Mit Blitzen bei 00, 11, 23, 34 und 45 Sekunden mit ungleichen
Zeitabständen nach dem Pseudo-Barker Kode ist nach schon unter einer
Minute der Minutenwechsel eindeutig. Die Minute ist in den 6 bit der
48-53-ten und 54-59-ten Sekunde ermittelbar. Kann verwendet werden, wenn
man nicht die Minute notieren möchte, die Sekunde ist aber schwerer
ablesbar. Paritätsbit für zusätzliche Sicherheit auf 46-ter Sekunde.
Sicherste Methode bei Gefahr durchziehender Wolken. Dann besser mehrere
Minuten Video aufnehmen.

**Video Auswertung:**

Die Blitze sind 40 ms lang (25 fps), sodaß die mit dem Drehknopf
eingestellte Helligkeit des Blitzes über alle längeren Belichtungszeiten
an einer Optik gleichbleibt. Die Blitze können so schwach eingestellt
werden, daß sie fast nicht sichtbar sind, die Auswertung nicht
beeinflussen aber dennoch sicher mit PyMovie (white aperture) und PyOTE
erkannt werden können. Bei maximaler Leuchtstärke sieht man den Blitz
sicher. Dies ist kaum abhängig von der Teleskopgröße!

PyOTE:
![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image5.png){width="3.042361111111111in"
height="1.0958333333333334in"}

**Elektrische Übersicht:**

![](D:\Daten\Arduino\_Projekte\GPS_flash\GPS_flash_fracture_GPS\media/media/image6.png){width="6.297916666666667in"
height="5.361805555555556in"}

Abbildung 3: Elektrischer Zusammenhang illustriert

**Ladegerät:**

Das integrierte Ladegerät wird über den USB-C Anschluß versorgt Eine
rote LED bedeutet, daß geladen wird, eine orange LED, daß der Akku
vollgeladen ist. Die Ladedauer beträgt etwa 2 Stunden.

**Quellen:**

\[1\] open-source code 1pps flasher\
<https://github.com/leontodon1/1pps_flasher>

**Version:**

Arduino: v61\
PCB: 1PPS_Teiler_v5_pro\
Datum: 2026/02

**Bildliste:**

Abbildung 1: GPS pps Blitzer, fertiges Gerät\
Abbildung 2: Elektrischer Zusammenhang illustriert\
Abbildung 3: Schaltplan der Verbindungsleiterplatte mit EAGLE

**Glossar:**

Arduino NANO Mikrocontroller Typ\
BCD Binary Coded Decimals, 0 bis 10 binär kodiert auf 4 bit\
DC-DC Direct Current, Gleichstromwandler\
EXTA Exposure Timing Analyzer\
GPS Global Positioning System\
LED Light Emitting Diode, Leuchtdiode\
LiFePo4 Lithiumeisenphosphat Akkutyp\
LSB Least Significant Bit, niedrigst wertiges Bit\
MSB Most Significant Bit, höchst wertiges Bit\
NMEA National Marine Electronics Association, Standard für GPS Daten\
open source Offen, frei verfügbar, darf verändert werden\
pps, 1pps peak per second, Signal pro Sekunde\
USB-C Universal Serial Bus, Typ C, hier nur für Energieversorgung\
VTI Video Time Inserter, Gerät um Zeitstempel zu erzeugen
