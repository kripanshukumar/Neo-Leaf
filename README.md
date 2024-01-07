# Description:
NEO LEAF is a device that sits between the main power supply and the general household appliances. The main work of NEO LEAF is to smartly create a wireless network or join the existing one to create a local connection between other NEO LEAF devices. This local connection allows the sharing of information like device status, power consumption, and active time. This data is crucial for smartly turning devices on or off where the total available power is limited and manual monitoring is impossible.

# Components Used:
ESP8266 32-bit MCU clocked at 128Mhz, with 2 MB SPI FLASH
PZEM-004T AC Multi-function Electric Energy Meter Power Monitor
Relay Switch 5V Relay Switch

<p align="center"><img src="https://github.com/kripanshukumar/Neo-Leaf/blob/main/Images/NodeMCU-ESP8266.jpg" width=60% height=60%></p>

# Learnings/ Skills gained:
* UART Communication: Implementing communication between different devices using the UART (Universal
Asynchronous Receiver-Transmitter) protocol.
* HTML, CSS & JS: Utilizing HTML, CSS, and JavaScript for creating web-based user interfaces and interactivity.
* File Operations using C++: Performing file operations, such as reading and writing, using the C++ programming language.
* UDP & TCP Protocols: Understanding and utilizing the UDP (User Datagram Protocol) and TCP (Transmission Control Protocol) for network communication.
* SSDP: Implementing the Simple Service Discovery Protocol for automatic service discovery on the local network.
* HTTP Client and Webserver on ESP8266: Developing an HTTP client to send requests and retrieve data, as well as a web server to handle incoming requests on the ESP8266 device.

# Operation:
* Esp8266 is used here as the heart of the system because not only does it provide all the common communication protocols like I2C, SPI, UART, and I2S but it also has a 2.4Ghz modem built which enables it to connect to the wireless network or created its network.
* When the ESP8266 starts for the first time, it starts in access point mode in which we can use our smartphones to connect to a temporary network and set up the credential for the first time.
* After connecting to the ESP8266 WIFI network, we use Chrome to go to the default settings page of the device where we are required to enter the credentials to create or connect to the WIFI network.

<p align="center"><img src="https://github.com/kripanshukumar/Neo-Leaf/blob/main/Images/WiFi%20Config.png" width=60% height=60%></p>

* Next step is to create a neo network or connect to an existing neo network. Connecting to a neo-network allows us to scan the other neo-devices in the same neo-network and avoid any other device that does not have the same network attribute or any suspicious device.

 <p align="center"><img src="https://github.com/kripanshukumar/Neo-Leaf/blob/main/Images/Neo-Network.png" width=60% height=60%></p>

 * After being a part of neo network, now we need to set up the device-related configuration settings such as attached appliance, appliance usage priority, input voltage, utility company, rate plan, peak demand (plan limit), and user target. These parameters help the device to smartly manage the operation of devices.

 <p align="center"><img src="https://github.com/kripanshukumar/Neo-Leaf/blob/main/Images/Lan%20Config.png" width=60% height=60%></p>

 * After setting all these parameters the device is ready to work in the neo network, it will share its power usage, and operation status and also poll the same data from the neo network.

# Technologies Used
1. ESP8266 Async Web Server (TCP): The ESP8266 Async Web Server is utilized to enable the ESP8266 module to function as a web server, hosting a collection of HTML files. These HTML files are responsible for capturing Wi-Fi credentials and device parameters. The Async Web Server supports HTTP_POST and HTTP_GET methods, allowing the device to receive and respond to information exchanged between the server and clients.
2. ESP8266 SSDP: SSDP (Simple Service Discovery Protocol) is implemented on the ESP8266 to enable automatic discovery and advertisement of services provided by the module on the local network. SSDP operates over the UDP (User Datagram Protocol) and follows a client-server model. The ESP8266, acting as a server, responds to SSDP discovery requests from client devices, facilitating the discovery and interaction with its services.
3. TCP (Transmission Control Protocol): TCP is a widely used transport layer protocol in computer networks. It ensures reliable, connection-oriented communication between devices over an IP-based network. TCP guarantees the correct order and error-free transmission of data packets, incorporating flow control mechanisms to manage network conditions effectively.
4. UDP (User Datagram Protocol): UDP is a lightweight transport layer protocol that operates alongside the IP in computer networks. It provides a connectionless communication method for transmitting datagrams (packets) between network devices. UDP does not offer reliability guarantees like TCP and is suitable for scenarios where low latency and lightweight communication are prioritized.
