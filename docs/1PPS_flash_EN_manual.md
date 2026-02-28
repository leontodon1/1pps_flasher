<h1 style="line-height:1.1; margin:0;">
Device with GPS pps flash<br>
for setting optical time stamps<br>
in astronomical videos from digital cameras
</h1>

<h2 style="margin-top:0.5em;">
Björn Kattentidt - IOTA/ES - <a href="mailto:bjoern@kattentidt-astro.de">bjoern@kattentidt-astro.de</a>
</h2>

**Motivation:**

Previous analogue cameras could be stamped with time information in the video data stream using VTI (video time inserter). Modern digital cameras only allow this with complex programming. This has resulted in many very good homemade camera systems, but at considerable expense. With state-of-the-art ‘smart telescopes’, the problem becomes clear that it will be even more difficult to meet the special requirements for time stamps with millisecond accuracy in videos.

A device that can be attached to any optical system and, when switched on, uses NMEA data after a few minutes to meet the requirements for evaluating time stamps in star occultation videos will be of interest. Such a device is presented here.

The system is published with open source code \[1\].

The device is intended for DIY construction. However, the author is happy to provide assistance with sourcing parts and assembling the finished device.\
This publication is intended to help you build the device yourself.

**Functional features at a glance:**

- Optical timestamps in astronomical videos

- Initial solution if the camera system does not provide time stamps 

- Secondary solution for timestamps in occultation videos if the primary timing system suddenly fails for technical reasons

- Super easy to use: Turn it on, pick a mode, wait a minute, and you're ready to go 

- pps flash (40 ms) via LED on clamp with cable (approx. 1 m) to control unit

- Brightness adjustable via rotary potentiometer with integrated switch

- Time flash distance adjustable over a wide range with two BCD encoding wheels

- Special functions: No settings required, GPS is used for GPS-synchronised flash patterns. If the GPS signal is invalid, the internal Arduino LED changes every second and the LED on the clip flashes every second at pps.

- Various modes preset in the upper number range, up to 100 modes possible

- Step-by-step instructions for DIY construction on github.com \[1\]

- Eyelet for hanging with correct position of the GPS antenna

- Strap for hanging and securing the LED cable

<div style="display: flex; align-items: flex-start; gap: 5px;">
<img src="media/image1.jpeg" alt="" width="30%">
<img src="media/image2.jpeg" alt="" width="30%">
<img src="media/image3.jpeg" alt="" width="30%">
</div>
<p>Figure 1: GPS pps-flasher, finished device</p>

**Technical features at a glance:**

- 3.2 V LiFePo4 battery with 600 mAh, consumption: typically 80 mA

- Operating time approx. 7 hours

- Last 0.5 to 1 hour: ‘low batt.’ indicator flashes rapidly

- Deep discharge protection: device switches off, residual current 2mA, all LEDs off

- 2 hours charging via USB-C, integrated charger.\
  Lower window: Red light ‘Charging’, orange light ‘Full’

- Arduino NANO with open source code via DC-DC converter (3.2 V to 5.0 V)

- GPS module ATGM336H (3.2 V) GPS+BD\
  Warm start 10-25 seconds (author), cold start 35 seconds (manufacturer),\
  up to several minutes in case of poor reception\
  Middle window: pps GPS ready indicated by inverted flashing LED

**Functionality:**

A flashing LED is attached to a clamp. This is connected to the control unit by a cable to reduce the mechanical load on the optics and enable wobble-free operation.

The clamp is attached to the dew cap of the optics with the LED facing inwards in the beam path. If the distance to the edge of the beam path is large, the clamp width can be modified by extending the black cylinder.

The code is optimised so that the flashes appear less than 100 µs after the pps signal (peak per second) from the GPS. The loop time is less than 0.25 ms. The light signal is therefore delayed by significantly less than 1 ms. Our own measurements using a DVTI yielded a maximum of 2 ms. However, this has not yet been reliably verified and still needs to be confirmed using EXTA!

Except for \#00, the flash always has the same duration of 40 ms.

In mode \#00, the LED lights up for a long time. This allows the brightness to be adjusted using the switching potentiometer. The faint glow can just about be seen in the video. PyOTE \[2\] can even evaluate markings when the flashing is no longer visible.

The special functions (#82-#99), on the other hand, use NMEA data, including for the second, while the time trigger is based on the significantly more accurate pps signal.

Only with valid NMEA data will the special functions work as expected and the upper status LED on the Arduino NANO go out! If the NMEA data is invalid or missing, the upper status LED will flash slowly (changing every second).

When the battery is fully charged, the device will flash for approx. 7 hours, with the status LED flashing rapidly for the last half hour to hour. The deep discharge protection then puts the device into sleep mode with a residual current of 2 mA. However, the device must still be switched off completely with the switch to protect the battery! The battery has a capacity of 600 mAh with a maximum consumption of 80 mA. The battery is charged via the USB-C port. A charger is integrated for this purpose. The control unit can be charged via the USB port when switched off or during operation. However, it is better not to charge during operation to protect the GPS module, which is then connected to the battery at increased voltage. The charging time is approximately two hours.

Deep discharge protection is implemented. At low voltage (3.1 V), the upper status LED flashes rapidly (4 hertz). At the end-of-discharge voltage (2.8 V), the device switches to sleep mode. When deep discharge protection is activated, all LEDs go out. However, a residual current still flows, causing the battery to be deeply discharged after another 2 days. <u>Therefore, the device must always be switched off using the switch, as a single deep discharge can destroy the LiFePo4 battery.</u>\
The device can be reactivated by switching it off using the rotary knob and then switching it back on again. If it switches off again immediately, wait a few seconds before switching it on again.

**Normal use:**

Hang the device from the eyelet or set it up with the eyelet facing upwards so that the GPS has optimal reception.

Clip the bracket onto the dew cap with the LED pointing into the optical path.

Switch on by turning the knob to the right.

Initially, the 1pps LED is permanently lit and the status LED flashes every second because no pps signal and no NMEA data have been detected yet. When the status LED is off and the 1pps signal briefly goes out very short every second, the device is ready for operation.\
This will happen within a minute in the open air, but sometimes it takes several minutes.

If the status LED flashes slowly, no valid NMEA data is available yet. This means that the special functions from \#99 down to \#82 cannot be used. However, the exact 1pps signal can be used with functions \#01 to \#81 if the 1pps LED on the device is already flashing. If the upper status LED flashes quickly, this means that the battery will soon be empty and the device will then switch to sleep mode.

The exact electrical pps signal from the GPS module is used for the LED on the clip.

First set the coding wheels to \#00 and adjust the required brightness on the potentiometer.

Then set the desired mode. You can switch to any other mode at any time using the two coding wheels. After one second, the signal will be correct.

The device can now be used to set millisecond-accurate timestamps visually.

<u>After use, switch off the device again using the rotary knob!</u>

**How it works:**

A red LED flashes briefly at the tip of the clamp every second, synchronized with GPS. If the number is set to \#11, for example, the LED flashes every eleventh second. This allows any cyclical repetition to be set and, with a little practice, the start can also be preset with the aid of a very accurate clock by quickly increasing the number starting at \#01.

To clarify: An internal counter counts up from zero after a flash until it reaches the set number. If you set this number higher after a flash at a specific second, you have set the sequence to the beginning.

Until the set number is exceeded by the counter, you can continue to adjust the coding wheels. With a little skill and thought, such as setting the tens first, there is enough time to complete the settings.

This gives you a lot of freedom, but this type of setting requires skill and you still need an accurate time (Android: e.g., AtomUhr app). If you start and stop the video with a robot app (e.g., MacroDroid) when using a “smart telescope” with a cell phone, the start and end of the video are accurate to the second, which makes it much easier to determine the minute.

For significantly easier use without additional apps, special functions are implemented from \#99 downwards (see list). These modes use NMEA data, which means that the flash always lights up exactly at the set sequence according to the GPS second!

The special functions and longer flash intervals are only suitable for cameras that can be relied upon for time synchronization, such as ASI cameras, while they are less suitable for QHY cameras, which appear to have firmware that causes the actual exposure times to fluctuate and unpredictable pauses to occur (author's own experience). However, for exposure times longer than 0.1 seconds, this is not very relevant for accurate timestamps.

If you are in a hurry, you must use a manually adjustable flash sequence and synchronize the start yourself with an accurate reference clock (Android: e.g., AtomUhr APP).

**List of available modes (expandable)**

\#00: Only for the purpose of adjusting the brightness of the LED on the clip.\
The LED lights up for 0.8 seconds after each pps.

\#01 ... \#81: The LED flashes at the set interval in seconds. It remains off in between. A specific setting can be achieved as described in the section “How it works”.

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
<th style="text-align: center;">flashes at specified GPS second</th>
<th style="text-align: center;">LUT</th>
<th style="text-align: center;">(Kode)</th>
<th style="text-align: center;"></th>
</tr>
</thead>
<tbody>
<tr>
<td colspan="5" style="text-align: center;">Second mask fully synchronized</td>
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
<td colspan="5" style="text-align: center;">Second mask with omission at zero crossing (minute change)</td>
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
<td colspan="5" style="text-align: center;">Second mask time-coded</td>
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
<td style="text-align: center;">00, 11, 23, 34, 45 (similar Pseudo-Barker Kode)</td>
<td style="text-align: center;">LUT_DE</td>
<td style="text-align: center;">DE</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td style="text-align: center;">#86</td>
<td style="text-align: center;">00, 11, 23, 34, 45; Minute binary: 48-53 und 54-59,<br />
LSB starting, parity bit 46</td>
<td style="text-align: center;">LUT_DD</td>
<td style="text-align: center;">DD</td>
<td style="text-align: center;"></td>
</tr>
<tr>
<td colspan="5" style="text-align: center;">Second mask without lead-in event before the minute change</td>
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
<td style="text-align: center;">Second interval, starting with any second</td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
<td style="text-align: center;"></td>
</tr>
</tbody>
</table>

Table 1: Modes

Sequences \#94 and \#99 enable GPS-synchronized flashes at equidistant intervals.

Sequences \#89 to \#93 do not have a flash at GPS second 0 for classifying the flashes relative to the minute.

Sequences \#88 and \#87 allow reliable time data to be determined and checked with PyMovie/PyOTE \[2\] if the minute of the start or end of the video is known.

Sequence \#86 is the same as \#87 but with additional integrated encoding of the minute with repetition and parity bit at fixed positions, similar to the method used in the DCF77 signal. This eliminates the need to note the minute.

Example:\
Even parity: Total number of ones with parity is even\
1 0 0 0 1 1 binary = 35 decimal: 35th minute\
MSB x x x x LSB, LSB comes first in time\
The parity bit is therefore calculated for even parity so that the total number of ones is even.

<img src="media\image4.png" alt="" width="100%">
<p>Figure 2: Mode #86, minute 35</p>

<img src="media\image4b.png" alt="" width="100%">
<p>Figure 3: Mode #86, minute 39, brightest flash, real measurement (C11) using PyOTE, C11</p>

<img src="media\image4c.png" alt="" width="100%">
<p>Figure 4: Mode #86, minute 23, weak flash, real measurement (C11) using PyOTE</p>

Sequences \#82 to \#85 have this missing flash at the end of the minute sequence. This makes it easier to determine the start of the minute.

The advantage of the special functions is that the light curve is less disturbed by the flashes than if there were a flash every second.\
Any other encodings can be implemented.

The disadvantage of the special functions is that the reception of the NMEA data must be valid, which can take longer or may be interrupted if reception is poor.

If only the pps signal is used in modes \#01 to \#81, it is sufficient for the 1pps-GPS LED to flash.

**Four useful applications:**

\#01:\
LED flashes every second. Basic function. Use when a precise second signal is required very quickly, or when images are lost at high frame rates, or when the camera's image times jitter. The minute change and minute must be determined differently, e.g., by noting the minute and the start of the video automatically at a specific GPS second.

\#85:\
LED flashes every 5 seconds except in the 55th second. If the flashes are clearly visible, the minute change can be easily evaluated. Quick replacement solution in case of primary system failure. The minute must be noted.

\#88:\
LED flashes at 00, 05, 10, 20, and 40 seconds. With few flashes, videos longer than 2 minutes can be reliably recognized when the minute changes. The minute must be noted.

\#86\
With flashes at 00, 11, 23, 34, and 45 seconds at unequal intervals according to the pseudo-Barker code, the change of minutes is clear after less than a minute. The minute can be determined in the 6 bits of the 48-53rd and 54-59th seconds. Can be used if you do not want to note the minute, but the second is more difficult to read. Parity bit for additional security at 46th second. Safest method in case of risk from passing clouds. In this case, it is better to record several minutes of video.

**Video evaluation:**

The flashes are currently 40 ms long (25 fps), so that the brightness of the flash set with the rotary knob remains the same across all exposure times on one optic. The flashes can be set so low that they are almost invisible, do not affect the evaluation, but can still be reliably detected with PyMovie (white aperture) and PyOTE \[2\]. At maximum brightness, the flash is clearly visible. This is hardly dependent on the size of the telescope!

PyOTE: <img src="media/image5.png" alt="" width="40%">

**Electrical overview:**

<img src="media/image6.png" alt="" width="80%">
<p>Figure 5: Electrical connection illustrated</p>

**Summary:**

It has been shown that a simple idea can be used to create a cost-effective method for accurate time stamps in astronomical videos in the range of less than a millisecond for any optics. The process of building your own device has been shown in detail \[1\]. The source code is available \[1\]. The author provides support with problems relating to the procurement of parts (suppliers) and has several devices for his own use, which can be loaned out for testing purposes.

**Sources:**

\[1\] open-source code 1pps-flasher\
<https://github.com/leontodon1/1pps_flasher>

\[2\] PyMovie, PyOTE\
<https://occultations.org/documents/Windows-PyMovie-PyOTE-Anaconda3-installation%202020%20updated%2012-2020%20.pdf>\
<https://pypi.org/project/pymovie/>

Translation with [deeple.com]\
<https://www.deepl.com/de/translator)>

**Version:**

Arduino: v61\
PCB: 1PPS_Teiler_v5_pro\
Datum: 2026/02

**List of figures:**

| Abbreviation | Description |
|--------|------------|
| Figure 1: | GPS pps-flasher, finished device  |
| Figure 2: | Mode #86, Minute 35             |
| Figure 3: | Mode #86, minute 39, brightest flash, real measurement (C11) using PyOTE, C11 |
| Figure 4: | Mode #86, minute 23, weak flash, real measurement (C11) using PyOTE |
| Figure 5: | Electrical connection illustrated |

**Glossar:**

| Abbreviation | Description |
|--------|------------|
| Arduino NANO | Microcontroller type |
| BCD | Binary Coded Decimals, 0 to 10 binary coded on 4 bits |
| C11 | Celestron 11 inch |
| DC-DC | Direct Current, DC converter |
| EXTA | Exposure Timing Analyzer |
| fps | fits per second |
| GPS | Global Positioning System |
| LED | Light Emitting Diode |
| LiFePo4 | Lithium iron phosphate battery type |
| LSB | Least Significant Bit |
| LUT | Look Up Table |
| MSB | Most Significant Bit |
| NMEA | National Marine Electronics Association, standard for GPS data |
| open source | Open, freely available, may be modified |
| pps, 1pps | peak per second, one signal per second |
| USB-C | Universal Serial Bus, type C, here only for power supply |
| VTI | Video Time Inserter, device for generating time stamps |
