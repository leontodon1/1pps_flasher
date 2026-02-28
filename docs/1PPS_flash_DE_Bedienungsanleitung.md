<h1 style="line-height:1.1; margin:0;">
Gerät mit GPS pps Blitz<br>
für das Setzen von optischen Zeitstempeln<br>
in astronomischen Videos von Digitalkameras
</h1>

<h2 style="margin-top:0.5em;">
Björn Kattentidt - IOTA/ES - <a href="mailto:bjoern@kattentidt-astro.de">bjoern@kattentidt-astro.de</a>
</h2>

**Motivation:**

Bisherige Analogkameras konnten mit VTI (video time inserter) im Videodatenstrom mit Zeitinformationen gestempelt werden. Moderne Digitalkameras ermöglichen dies nur mit aufwändiger Programmierung. Daraus ergaben sich viele auch sehr gute Kamerasysteme im Eigenbau mit erheblichem Aufwand. Mit modernsten „smart telescopes“ wird das Problem deutlich, daß es noch schwerer möglich sein wird die speziellen Wünsche nach Zeitmarken mit Genauigkeit im Bereich von Millisekunden in Videos zu realisieren.

Ein Gerät wird interessant sein, welches man an jede beliebige Optik anbringt und beim Anschalten nach wenigen Minuten NMEA-Daten verwendet um die Anforderungen für Auswertung von Zeitmarken in Sternbedeckungsvideos zu erfüllen. Ein solches wird hier vorgestellt.

Das System wird mit freiem Quellcode veröffentlicht.

Das Gerät ist für den Selbstbau (DIY) gedacht. Der Autor unterstützt aber gerne bei Hinweisen zur Teilebeschaffung und der Umsetzung zum fertigen Gerät.\
Diese Veröffentlichung soll dazu beitragen den Nachbau selbständig durchzuführen.

**Funktionale Eigenschaften im Überblick:**

-	Optische Zeitmarken in astronomischen Videos 

-	Erstlösung wenn das Kamerasystem keine Zeitmarken anbietet. 

-	Zweitlösung wenn das primäre Zeitsystem aus technischen Gründen plötzlich ausfällt 

-	Einfachste Bedienung: Einschalten, Modus wählen, 1 Minute warten, einsatzbereit

-	pps Blitz (40 ms) mittels LED an Klammer mit Leitung (ca. 1 m) zum Steuergerät

-	Helligkeit über Drehpotentiometer mit integriertem Schalter einstellbar

-	Zeitliche Blitzdistanz in weiten Bereichen mit zwei BCD Kodierrädern einstellbar

-	Viele Sonderfunktionen: Keine Einstellung notwendig, GPS synchrone  Blitzmuster\
  Keine NMEA Daten aber pps: Interne Arduino LED im Sekundentakt und LED an der Klammer blitzt jede Sekunde wie #01. 

-	Diverse Modi voreingestellt am oberen Zahlenbereich, bis 100 Modi möglich

-	Schritt für Schritt Anleitung zum Selbstbau auf github.com [1]

-	Öse zum Aufhängen mit richtiger Position der GPS Antenne

-	Band zum Aufhängen als auch zur Fixierung des LED Kabels

<div style="display: flex; align-items: flex-start; gap: 5px;">
<img src="media/image1.jpeg" alt="" width="30%">
<img src="media/image2.jpeg" alt="" width="30%">
<img src="media/image3.jpeg" alt="" width="30%">
</div>
<p>Abbildung 1: GPS pps Blitzer, fertiges Gerät</p>

**Technische Eigenschaften im Überblick**

-	3,2 V LiFePo4 Akku mit 600 mAh, Verbrauch: typisch 80 mA

-	Betriebsdauer ca. 7 Stunden

-	Letzte 0,5 … 1 Stunde „low batt.“ Anzeige durch schnell blinkende Status-LED 

-	Tiefentladeschutz: Gerät schaltet ab, Reststrom 2mA, alle LED aus

-	2h Laden über USB-C, Ladegerät integriert. \
  Unteres Fenster: Rotes Licht „Laden“, oranges Licht “Voll“

-	Arduino NANO mit open source Kode über DC-DC Wandler (3,2 V zu 5,0 V)

-	GPS Modul ATGM336H (3,2 V) GPS+BD\
  Warmstart 10-25 Sek. (Autor), Kaltstart 35 Sek. (Hersteller), \
  bis zu einigen Minuten bei ungünstigem Empfang\
  Mittleres Fenster: pps GPS Bereitschaft durch invertiert blinkende LED

**Funktionalität:**

Eine blitzende LED ist an einer Klammer angebracht. Diese ist mit einer Leitung mit dem Steuergerät verbunden um die mechanische Last an der Optik zu verringern und die Bedienung wackelfrei zu ermöglichen.

Die Klammer wird an die Taukappe der Optik mit LED nach innen in den Strahlengang angeklemmt. Bei großem Abstand zum Rand des Strahlengangs kann die Klammerweite modifiziert werden indem der schwarze Zylinder verlängert wird.

Der Code ist derart optimiert, daß die Blitze weniger als 100 µs nach dem pps Signal (peak per second) des GPS erscheinen. Die loop-Zeit beträgt unter 0,25 ms. Das Lichtsignal ist demnach deutlich weniger als 1 ms verzögert. Eigene Messungen mithilfe einer DVTI ergaben maximal 2ms. <span class="mark">Dies ist aber derzeit noch nicht zuverlässig nachgemessen worden und muß noch mittels EXTA festgestellt werden!</span>

Der Blitz hat außer bei \#00 immer dieselbe Dauer von 40ms.

Im Modus \#00 leuchtet die LED lang. So kann die Helligkeit mit dem Schaltpotentiometer eingestellt werden. Am besten sieht man gerade noch ein schwaches Aufleuchten im Video. PyOTE kann sogar Markierungen auswerten, wenn man das Blitzen nicht mehr sieht.

Die speziellen Funktionen (#82-#99) verwenden dagegen NMEA-Daten, auch für die Sekunde, während die zeitliche Triggerung über das deutlich genauere pps-Signal verwendet wird.\
Erst mit gültigen NMEA-Daten arbeiten die Sonderfunktionen wie erwartet und die obere obere Status-LED am Arduino NANO verlischt! Bei ungültigen oder keinen NMEA-Daten blinkt die obere Status-LED langsam (wechselt jede Sekunde).

Bei vollem Akku wird das Gerät ca. 7 Stunden blitzen wobei die letzte halbe bis eine Stunde die Status-LED schnell blinkt. Dann versetzt der Tiefentladeschutz das Gerät in den Ruhemodus mit 2 mA Reststrom. Das Gerät muß zum Schutz des Akkus dennoch mit dem Schalter vollständig abgeschaltet werden! Der Akku hat 600 mAh Kapazität bei max. 80 mA Verbrauch. Der Akku wird über den USB-C Anschluß geladen. Ein Ladegerät ist dazu integriert. Das Steuergerät kann im ausgeschalten Zustand oder auch während des Betriebs über die USB-Buchse geladen werden. Dennoch ist es besser während des Betriebs nicht zu laden zum Schutz des GPS-Moduls, welches dann mit dem Akku verbunden ist bei erhöhter Spannung. Die Ladezeit beträgt ca. zwei Stunden.

Ein Tiefentladeschutz ist implementiert. Bei geringer Spannung (3,1 V) blinkt die obere Status-LED in schneller Folge (4 Hertz). Bei Entladeschlußspannung (2,8 V) schaltet sich das Gerät in den Ruhemodus. Ist der Tiefentladeschutz aktiviert, gehen alle LED aus. Es fließt aber noch ein Reststrom wodurch der Akku nach weiteren 2 Tagen tiefentladen wird. <u>Daher muß das Gerät immer per Schalter ausgeschaltet werden, denn eine einzige Tiefentladung kann den LiFePo4 Akku zerstören.</u>\
Das Gerät kann reaktiviert werden, indem man es am Drehknopf ausschaltet, und wieder anschaltet. Geht es sofort wieder aus mit dem Anschalten ein paar Sekunden warten.

**Übliche Benutzung:**

Gerät an der Öse aufhängen oder mit Öse zum Zenit aufstellen, dann hat das GPS optimalen Empfang.

Die Klammer an der Taukappe anklemmen mit der LED im Strahlengang in die Optik zeigend.

Mit Drehknopf nach rechts einschalten.

Zunächst ist die 1pps-LED dauerhaft an und die Status-LED blinkt im Sekundenrhythmus, weil noch kein pps-Signal und keine NMEA Daten erkannt wurden. Wenn die Status-LED aus ist und das 1pps-Signal im Sekundenrhythmus kurz aus geht, ist das Gerät betriebsbereit.\
Dies wird unter freiem Himmel binnen einer Minute geschehen, manchmal dauert es mehrere Minuten.

Blinkt die Status-LED langsam, liegen noch keine gültigen NMEA Daten vor. Damit können die speziellen Funktionen von \#99 abwärts bis \#82 nicht verwendet werden. Allerdings kann das exakte 1pps-Signal mit den Funktionen \#01 bis \#81 verwendet werden, wenn schon die 1pps-LED blinkt. Blinkt die Status-LED schnell, bedeutet dies, daß der Akku bald leer sein wird und das Gerät dann in den Ruhemodus ausschaltet.

Für die LED an der Klammer wird das exakte elektrische pps-Signal des GPS-Moduls verwendet.

Die Kodierräder zunächst auf \#00 stellen und die benötigte Helligkeit am Potentiometer einstellen.

Dann den gewünschten Modus einstellen. Man kann jederzeit in jeden anderen Modus mit den beiden Kodierrädern schalten. Nach einer Sekunde ist das Signal dann korrekt.\
Nun kann das Gerät benutzt werden um Millisekunden genaue Zeitstempel optisch zu setzen.

<u>Nach der Verwendung das Gerät am Drehknopf wieder abschalten!</u>

**Funktionsweise:**

An der Spitze der Klammer blitzt im Sekundentakt eine rote LED GPS synchron kurz auf. Wird die Zahl zum Beispiel auf \#11 gestellt blitzt die LED jede elfte Sekunde. So kann jede zyklische Wiederholung eingestellt werden und der Beginn mit etwas Übung auch mit Hilfe einer sehr genauen Uhr voreingestellt werden, indem man die Zahl beginnend bei \#01 geschwind höher stellt.

Zum Verständnis: Ein interner Zähler zählt nach einem Blitzen von Null an aufwärts bis er die eingestellte Zahl erreicht hat. Stellt man diese Zahl nach einem Blitzen zu bestimmter Sekunde höher, hat man die Sequenz auf den Beginn eingestellt.\
Bis die eingestellte Zahl vom Zähler überschritten wird, kann man weiter die Kodierräder einstellen. Mit etwas Geschick und Überlegung, etwa den Zehner zuerst zu stellen, bleibt ausreichend Zeit mit den Einstellungen fertig zu werden.

Dies ergibt zwar viele Freiheiten, aber diese Art der Einstellung erfordert Geschick und man benötigt noch eine genaue Zeit (Android: z.B. AtomUhr App). Startet und stoppt man das Video mit einer Roboter App (z.B. MacroDroid) bei Verwendung eines „smart telescope“ mit Mobiltelefon, dann sind Beginn und Ende des Videos sekundengenau, was die Feststellung der Minute wesentlich erleichtert.

Zur deutlich leichteren Nutzung ohne weitere Apps sind Sonderfunktionen von \#99 herab implementiert (siehe Liste). Diese Modi nutzen NMEA-Daten und der Blitz leuchtet daher immer genau zur eingestellten Sequenz entsprechend der GPS Sekunde auf!

Die Sonderfunktionen als auch längere Blitzabstände sind nur geeignet für Kameras bei denen auf Zeit Synchronität Verlaß ist wie etwa bei ASI Kameras, während sie weniger geeignet sind für QHY Kameras, welche eine Firmware zu haben scheinen, wodurch die echten Belichtungszeiten fluktuieren sowie unvorhersehbare Pausen erscheinen (eigene Erfahrung des Autors). Bei Belichtungszeiten über 0,1 Sekunden ist das allerdings wenig relevant für genaue Zeitstempel.

Hat man es eilig, muß man eine manuell einstellbare Blitzfolge verwenden und den Beginn selbst passend mit einer genauen Vergleichsuhr synchronisieren (Android: z.B. AtomUhr APP).

**Liste der verfügbaren Modi (erweiterbar)**

\#00: Nur zu Zwecken der Helligkeitseinstellung der LED an der Klammer.\
Die LED leuchtet 0,8 Sekunden lang nach jedem pps.

\#01 … \#81: Die LED blitzt im Abstand der eingestellten Sekunden. Dazwischen bleibt sie aus. Eine gezielte Einstellung kann erreicht werden gemäß Beschreibung im Abschnitt Verwendung.

<table>
<colgroup>
<col style="width: 14%" />
<col style="width: 61%" />
<col style="width: 13%" />
<col style="width: 9%" />
<col style="width: 0%" />
</colgroup>
<thead>
<tr>
<th style="text-align: center;">BCD</th>
<th style="text-align: center;">Blitze bei genannter GPS-Sekunde</th>
<th style="text-align: center;">LUT</th>
<th style="text-align: center;">(Kode)</th>
<th style="text-align: center;"></th>
</tr>
</thead>
<tbody>
<tr>
<td colspan="5" style="text-align: center;">Sekundenmaske vollständig synchron</td>
</tr>
<tr>
<td style="text-align: center;">#99</td>
<td style="text-align: center;">00, 05, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55</td>
<td style="text-align: center;">LUT_05s</td>
<td style="text-align: center;">FF</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#98</td>
<td style="text-align: center;">00, 10, 20, 30, 40, 50</td>
<td style="text-align: center;">LUT_10s</td>
<td style="text-align: center;">FE</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#97</td>
<td style="text-align: center;">00, 15, 30, 45</td>
<td style="text-align: center;">LUT_15s</td>
<td style="text-align: center;">FD</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#96</td>
<td style="text-align: center;">00, 20, 40</td>
<td style="text-align: center;">LUT_20s</td>
<td style="text-align: center;">FC</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#95</td>
<td style="text-align: center;">00, 30</td>
<td style="text-align: center;">LUT_30s</td>
<td style="text-align: center;">FB</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#94</td>
<td style="text-align: center;">00</td>
<td style="text-align: center;"></td>
<td style="text-align: center;">FA</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="5" style="text-align: center;">Sekundenmaske mit Auslassung am Nulldurchgang (Minutenwechsel)</td>
</tr>
<tr>
<td style="text-align: center;">#93</td>
<td style="text-align: center;">05, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55</td>
<td style="text-align: center;">LUT_05s0</td>
<td style="text-align: center;">EF</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#92</td>
<td style="text-align: center;">10, 20, 30, 40, 50</td>
<td style="text-align: center;">LUT_10s0</td>
<td style="text-align: center;">EE</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#91</td>
<td style="text-align: center;">15, 30, 45</td>
<td style="text-align: center;">LUT_15s0</td>
<td style="text-align: center;">ED</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#90</td>
<td style="text-align: center;">20, 40</td>
<td style="text-align: center;">LUT_20s0</td>
<td style="text-align: center;">EC</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#89</td>
<td style="text-align: center;">30</td>
<td style="text-align: center;">LUT_30s0</td>
<td style="text-align: center;">EB</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="5" style="text-align: center;">Sekundenmaske zeitlich kodiert</td>
</tr>
<tr>
<td style="text-align: center;">#88</td>
<td style="text-align: center;">00, 05, 10, 20, 40</td>
<td style="text-align: center;">LUT_DF</td>
<td style="text-align: center;">DF</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#87</td>
<td style="text-align: center;">00, 11, 23, 34, 45 (ähnlich Pseudo-Barker Kode)</td>
<td style="text-align: center;">LUT_DE</td>
<td style="text-align: center;">DE</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#86</td>
<td style="text-align: center;">00, 11, 23, 34, 45; Minute binär: 48-53 und 54-59, LSB beginnend, Paritätsbit 46</td>
<td style="text-align: center;">LUT_DD</td>
<td style="text-align: center;">DD</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="5" style="text-align: center;">Sekundenmaske ohne Vorlaufereignis vor dem Minutenwechsel</td>
</tr>
<tr>
<td style="text-align: center;">#85</td>
<td style="text-align: center;">00, 05, 10, 15, 20, 25, 30, 35, 40, 45, 50</td>
<td style="text-align: center;">LUT_05sv0</td>
<td style="text-align: center;">CF</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#84</td>
<td style="text-align: center;">00, 10, 20, 30, 40</td>
<td style="text-align: center;">LUT_10sv0</td>
<td style="text-align: center;">CE</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#83</td>
<td style="text-align: center;">00, 15, 30</td>
<td style="text-align: center;">LUT_15sv0</td>
<td style="text-align: center;">CD</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#82</td>
<td style="text-align: center;">00, 20</td>
<td style="text-align: center;">LUT_20sv0</td>
<td style="text-align: center;">CC</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="5" style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#01 … #81</td>
<td style="text-align: center;">Sekunden Abstand, beginnend mit beliebiger Sekunde</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

Tabelle 1: Modi

Die Sequenzen \#94 und \#99 ermöglichen GPS-synchronisierte Blitze in äquidistantem Zeitabstand.

Die Sequenzen \#89 bis \#93 haben keinen Blitz bei GPS Sekunde 0 zur Einordnung der Blitze relativ zur Minute.

Die Sequenzen \#88 und \#87 erlauben es mit Wissen über die Minute des Beginns oder Ende des Videos zuverlässige Zeitangaben mit PyMovie/PyOTE zu ermitteln und zu prüfen.

Die Sequenz \#86 ist gleich wie \#87 aber mit zusätzlich integrierter Kodierung der Minute mit Wiederholung und Paritätsbit an fixen Positionen, ähnlich der Methode beim DCF77 Signal. Damit ist es nicht mehr nötig die Minute zu notieren.

Beispiel:\
Gerade Parität: Gesamtzahl der Einsen mit Parität ist gerade\
1 0 0 0 1 1 binär = 35 dezimal: 35-te Minute\
MSB x x x x LSB, LSB kommt zeitlich zuerst\
Das Paritätsbit wird also bei gerader Parität so berechnet, dass die Gesamtanzahl der Einsen gerade ist.

<img src="media\image4.png" alt="" width="100%">
<p>Abbildung 2: Modus #86, Minute 35</p>

<img src="media\image4.png" alt="" width="100%">
<p>Abbildung 3: Modus #86, Minute 39, hellster Blitz, reale Messung (C11) mit PyOTE</p>

<img src="media\image4.png" alt="" width="100%">
<p>Abbildung 4: Modus #86, Minute 23, kaum sichtbarer Blitz, reale Messung (C11) mit PyOTE</p>

Die Sequenzen \#82 bis \#85 haben diesen fehlenden Blitz an letzter Stelle in der Minutensequenz. Damit läßt sich der Minutenbeginn leichter feststellen.

Vorteil der Sonderfunktionen liegt in der geringeren Störung der Lichtkurve durch die Blitze im Gegensatz zu einem Blitz in jeder Sekunde.\
Jegliche anderen Kodierungen können implementiert werden.

Nachteil der Sonderfunktionen ist, daß der Empfang der NMEA-Daten gültig sein muß, was länger dauern kann oder auch bei schlechtem Empfang Unterbrechungen erfahren kann.

Benutzt man nur das pps-Signal in den Modi \#01 bis \#81 ist es ausreichend wenn die LED des GPS blinkt.

**Vier nützliche Anwendungsfälle:**

\#01:\
LED blitzt jede Sekunde. Grundfunktion. Verwendung wenn sehr schnell ein genaues Sekundensignal notwendig wird oder Bilder bei hohen Bildraten verloren gehen oder wenn die Bildzeiten der Kamera jittern. Es müssen Minutenwechsel und Minute anders ermittelt werden, z.B. durch Notieren der Minute sowie Beginn des Videos per Automatismus zu bestimmter GPS-Sekunde.

\#85:\
LED blitzt alle 5 Sekunden außer in der 55-ten Sekunde. Bei gut erkennbaren Blitzen ist der Minutenwechsel gut auswertbar. Schnelle Ersatzlösung bei Versagen des Primärsystems. Minute muß notiert werden.

\#88:\
LED blitzt bei 00, 05, 10, 20 und 40 Sekunden. Mit wenig Blitzen hat man bei mehr als 2 Minuten langen Videos eine sichere Erkennung des Minutenwechsels. Minute muß notiert werden.

\#86\
Mit Blitzen bei 00, 11, 23, 34 und 45 Sekunden mit ungleichen Zeitabständen nach dem Pseudo-Barker Kode ist nach schon unter einer Minute der Minutenwechsel eindeutig. Die Minute ist in den 6 bit der 48-53-ten und 54-59-ten Sekunde ermittelbar. Kann verwendet werden, wenn man nicht die Minute notieren möchte, die Sekunde ist aber schwerer ablesbar. Paritätsbit für zusätzliche Sicherheit auf 46-ter Sekunde. Sicherste Methode bei Gefahr durchziehender Wolken. Dann besser mehrere Minuten Video aufnehmen.

**Video Auswertung:**

Die Blitze sind derzeit 40 ms lang (25 fps), sodaß die mit dem Drehknopf eingestellte Helligkeit des Blitzes über alle Belichtungszeiten an einer Optik gleichbleibt. Die Blitze können so schwach eingestellt werden, daß sie fast nicht sichtbar sind, die Auswertung nicht beeinflussen aber dennoch sicher mit PyMovie (white aperture) und PyOTE erkannt werden können. Bei maximaler Leuchtstärke sieht man den Blitz sicher. Dies ist kaum abhängig von der Teleskopgröße!

PyOTE: <img src="media/image5.png" alt="" width="40%">

**Elektrische Übersicht:**

<img src="media/image6.png" alt="" width="80%">
<p>Abbildung 5: Elektrischer Zusammenhang illustriert</p>

**Zusammenfassung:**

Es wurde gezeigt, daß mit einer einfachen Idee eine kostengünstige Methode für exakte Zeitstempel in astronomischen Videos im Bereich unter einer Millisekunde für jegliche Optik möglich ist. Der Weg zum eigenen Gerät im Zuge des Selbstbaus wurde detailliert gezeigt. Der Quellcode wird zur Verfügung gestellt \[2\]. Der Autor unterstützt bei Problemen mit der Teilebeschaffung (Lieferanten) und hat mehrere Geräte zur eigenen Nutzung, die zum Test leihweise abgegeben werden.

**Quellen:**

\[1\] EAGLE, <https://en.eagle.cool/download>

\[2\] open-source code 1pps flasher\
<https://github.com/leontodon1/1pps_flasher>

**Version:**

Arduino: v61\
PCB: 1PPS_Teiler_v5_pro\
Datum: 2026/02

**Bildliste:**

| Abbildung | Beschreibung |
|--------|------------|
| Abbildung 1: | GPS pps Blitzer, fertiges Gerät |
| Abbildung 2: | Modus #86, Minute 35 |
| Abbildung 3: | Modus #86, Minute 39, hellster Blitz, reale Messung (C11) mit PyOTE |
| Abbildung 4: | Modus #86, Minute 23, kaum sichtbarer Blitz, reale Messung (C11) mit PyOTE |
| Abbildung 5: | Elektrischer Zusammenhang illustriert |

**Glossar:**

| Abkürzung | Beschreibung |
|--------|------------|
| Arduino NANO | Mikrocontroller Typ |
| BCD | Binary Coded Decimals, 0 bis 10 binär kodiert auf 4 bit |
| C11 | Celestron 11 Zoll |
| DC-DC | Direct Current, Gleichstromwandler |
| EXTA | Exposure Timing Analyzer |
| GPS | Global Positioning System |
| LED | Light Emitting Diode, Leuchtdiode |
| LiFePo4 | Lithiumeisenphosphat Akkutyp |
| LSB | Least Significant Bit, niedrigst wertiges Bit |
| LUT | Look Up Table, Nachschlagetabelle |
| MSB | Most Significant Bit, höchst wertiges Bit |
| NMEA | National Marine Electronics Association, Standard für GPS Daten |
| open source | Offen, frei verfügbar, darf verändert werden |
| pps, 1pps | peak per second, Signal pro Sekunde |
| USB-C | Universal Serial Bus, Typ C, hier nur für Energieversorgung |
| VTI | Video Time Inserter, Gerät um Zeitstempel zu erzeugen |
