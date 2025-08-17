# Plant Health Monitoring System with LoRaWAN connectivity

**Plant Health Monitoring System with LoRaWAN connectivity** is a Cyber-Physical IoT System designed to monitor and transmit plant health and environmental conditions over a **LoRaWAN network**. Using the **B-L072Z-LRWAN1 ARM Mbed platform** and the **ResIOT.io server**, the system measures multiple parameters (temperature, humidity, soil moisture, brightness, GPS location) and enables remote actuation (RGB LED control).

---

## Features

### Sensors
- Temperature Sensor – measures ambient temperature  
- Humidity Sensor – monitors relative humidity  
- Soil Moisture Sensor – evaluates water content in soil  
- Light Sensor – tracks light intensity  
- GPS Module – provides plant location  

### Actuators
- RGB LED – indicates system state, remotely controlled via LoRaWAN  
- Serial Debug Output – for testing payloads without network connection  

---

## System Requirements
- **Hardware**  
  - B-L072Z-LRWAN1 Discovery Kit (ARM Mbed)  
  - Sensor modules: Temperature, Humidity, Soil Moisture, Light, GPS  
  - USB cable + PC for programming  

- **Software**  
  - [Mbed Studio](https://os.mbed.com/studio/)  
  - ResIOT.io account 
  - Serial terminal 
  - Lua scripting for ResIOT server  

---

## Pin Connections

| Component       | Connection (example) | Description                          |
|-----------------|----------------------|--------------------------------------|
| Soil Moisture   | ADC Pin              | Analog read                          |
| Temperature     | I2C (SDA/SCL)        | Digital read                         |
| Humidity        | I2C (SDA/SCL)        | Digital read                         |
| Light Sensor    | ADC Pin              | Analog read                          |
| GPS Module      | UART                 | NMEA data transfer                   |
| RGB LED         | GPIO                 | Remote actuation (ON/OFF/Color)      |

*(Pin mappings may vary depending on your specific setup and Mbed code.)*

---

## Usage

### Data Transmission
- The node sends encoded payloads (≤30 bytes) containing:  
  - Temperature  
  - Humidity  
  - Soil Moisture  
  - Light level  
  - GPS coordinates  

### Data Visualization
- Log in to [ResIOT.io](https://eu72.resiot.io/)  
- Add your node   
- Attach the provided **Lua script** to decode payloads  
- View real-time values on the **ResIOT dashboard** or **mobile app**  

### Remote Control
- From ResIOT, send commands to control the RGB LED:  
  - `OFF` – Turn LED off  
  - `Red` – Turn LED red  
  - `Green` – Turn LED green  

---

## Authors
- Marilena Tsami
- Fedi Khayati

---

## License
This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.  
