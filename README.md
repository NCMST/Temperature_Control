<h1>Temerature controller on ESP32 using MAX6675</h1>

<h2>Hardware</h2>

<p>This project utilizes the following hardware components:</p>

<ul>
    <li><code>ESP32-S3</code> <a href="https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html">link for more info</a> or <a href="https://github.com/WeActStudio/WeActStudio.ESP32S3-AorB">link for a spesific WeActStudio board</a></li>
    <li><code>ESP32 DevKitC V4</code> <a href="https://docs.espressif.com/projects/esp-idf/en/stable/esp32/hw-reference/esp32/get-started-devkitc.html">link for more info</a></li>
    <li>MAX6675 thermocouple module</li>
    <li>Reset button for Wi-Fi</li>
    <li>SSD1306 OLED display</li>
    <li>Optocoupler with TRIAC for thermal reactor control connected to 220VAC</li>
    <li>Zero cross detector circuit on the same PCB for phase detection and PWM control of reactor power</li>
</ul>

<h2>Software</h2>

<p>This project utilizes the following software components:</p>

<ul>
    <li>FreeRTOS for task scheduling and management</li>
    <li>Web interface created for ESP32 displaying real-time graphs</li>
    <li>Printing temperature and time values in a table</li>
    <li>Allowing the setting of PID constants for PWM signal</li>
</ul>

<h2>Code</h2>

<p>The actual code can be found in folder <code>v.0.1</code>, under <a href="/Software/Temperature-control/">MCUCod</a>.</p>

<h2>Circuit and PCB Design</h2>

<p>The circuit and PCB design can be found on EasyEDA:</p>

<a href="https://oshwlab.com/creciunelcatalin/controlul-temperaturii">EasyEDA - Circuit and PCB</a>

<h2>Additional Notes</h2>

<p>This project also features an application for use via USB from a PC or Bluetooth, providing versatile control options.</p>

<p>The circuit design takes into account the form and size of the case for practical implementation.</p>
