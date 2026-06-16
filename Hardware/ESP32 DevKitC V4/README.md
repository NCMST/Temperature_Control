<h1>ESP32 Thermal Reactor Controller (Zephyr RTOS)</h1>

<p>This repository contains an embedded application designed for precise temperature control inside a thermal reactor. The system runs on <strong>Zephyr RTOS (v4.2.0)</strong> utilizing an <strong>ESP32-DevKitC V4</strong> development board.</p>

<p>The firmware implements a closed-loop <strong>PID</strong> regulation mechanism to sustain steady target temperatures. Thermal feedback is managed via two independent sensing methods (Thermocouple inside the chamber and an ambient NTC thermistor). Output data is rendered onto a local OLED display, while mains power regulation (220VAC) is handled via a Phase-Cut method driven by a dedicated Zero Cross Detection circuit. Remote configuration and tracking parameters are accessible over <strong>Wi-Fi</strong>.</p>

<h2>Hardware Components</h2>

<ul>
    <li><strong>Microcontroller:</strong> <a href="https://docs.espressif.com/projects/esp-idf/en/v5.1/esp32/hw-reference/esp32/user-guide-devkitc-v4.html">ESP32 DevKitC V4</a></li>
    <li><strong>Chamber Temperature Sensor:</strong> <strong>MAX6675</strong> thermocouple interface module mapped via SPI2.</li>
    <li><strong>Ambient Temperature Sensor:</strong> <strong>NTC</strong> Thermistor read via ADC1 for local reference monitoring.</li>
    <li><strong>Local Display:</strong> <strong>SSD1306 OLED</strong> display (128x64 resolution, communicating over I2C0).</li>
    <li><strong>Power Actuator (220VAC):</strong> Optocoupler with <strong>TRIAC (MOC)</strong> to drive the reactor heating element safely.</li>
    <li><strong>AC Synchronization:</strong> Onboard <strong>Zero Cross Detector (ZCD)</strong> circuit connected to a hardware input for phase tracking and precise PWM power matching.</li>
</ul>

<img src="https://docs.espressif.com/projects/esp-idf/en/v5.1/esp32/_images/esp32-devkitC-v4-pinout.png" alt="ESP32-DevKitC Pinout Diagram">

<h2>Hardware Pin Configuration</h2>

<h3>1. I2C0 Interface (SSD1306 OLED Display)</h3>

<p>Display Parameters: Address: <code>0x3C</code> | Resolution: <code>128x64 px</code> | Reset Pin: <code>-1 (Disabled)</code></p>

<table>
    <thead>
        <tr>
            <th>Physical Pin</th>
            <th>Interface Bus</th>
            <th>Hardware Function</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><strong>21</strong></td>
            <td>I2C0</td>
            <td>SDA (Data Line)</td>
        </tr>
        <tr>
            <td><strong>22</strong></td>
            <td>I2C0</td>
            <td>SCL (Clock Line)</td>
        </tr>
    </tbody>
</table>

<h3>2. SPI2 Interface (MAX6675 Thermocouple Module)</h3>

<table>
    <thead>
        <tr>
            <th>Physical Pin</th>
            <th>Interface Bus</th>
            <th>Hardware Function</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><strong>14</strong></td>
            <td>SPI2</td>
            <td>SCK (Serial Clock)</td>
        </tr>
        <tr>
            <td><strong>12</strong></td>
            <td>SPI2</td>
            <td>MISO (Master Input Slave Output)</td>
        </tr>
        <tr>
            <td><strong>15</strong></td>
            <td>SPI2</td>
            <td>CS (Chip Select)</td>
        </tr>
    </tbody>
</table>

<h3>3. Peripherals & Power Control Signals</h3>

<table>
    <thead>
        <tr>
            <th>Physical Pin</th>
            <th>Pin Type</th>
            <th>Signal Name</th>
            <th>Functional Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><strong>23</strong></td>
            <td>Output (PWM)</td>
            <td><code>MOC_PIN</code></td>
            <td>TRIAC gate control driver pin (Heating Control)</td>
        </tr>
        <tr>
            <td><strong>36</strong></td>
            <td>Input (Digital)</td>
            <td><code>ZCD_PIN</code></td>
            <td>Zero Cross Detector input (Mains 220V Phase Sync)</td>
        </tr>
        <tr>
            <td><strong>39</strong></td>
            <td>Input (ADC1_CH3)</td>
            <td><code>NTC_PIN</code></td>
            <td>Analog read line for Negative Temperature Coefficient thermistor</td>
        </tr>
        <tr>
            <td><strong>2</strong></td>
            <td>Output</td>
            <td><code>LED</code></td>
            <td>Onboard status LED indicator</td>
        </tr>
    </tbody>
</table>

<blockquote>
    <h3>Critical Firmware Code Review Notes</h3>
    <p>The following logic discrepancies were identified within your current <code>src/main.cpp</code> architecture:</p>
    <ul>
        <li><strong>Temperature Unit Bug:</strong> The <code>read_ntc_temp()</code> function evaluates values directly to Celsius using the Steinhart-Hart equation (subtracting <code>273.15</code> at the return step). However, inside the <code>main()</code> routine loop, this output variable is incorrectly handled as Fahrenheit and run through an additional mathematical conversion step: <code>(temp_F - 32) * (5.0 / 9.0)</code>. This corrupts log readouts.</li>
        <li><strong>Zephyr Portability Standard:</strong> Hardcoding physical integers like <code>MOC_PIN 23</code> directly inside configuration routines bypasses Zephyr's device-agnostic layer. For standard compliance and stable multi-port setups, this mapping should be bound via <code>gpio_dt_spec</code> wrappers configured from the overlay tree layout.</li>
    </ul>
</blockquote>