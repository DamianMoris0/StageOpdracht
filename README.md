# Stage Opdracht

## Intro
In this repo you will find code to read temperature and pressure data from a BMP180 sensor with a Raspberry Pi Zero 2 W and save it in RAM memory in a JSON file.
This code will also be unit tested using [Unity](https://github.com/ThrowTheSwitch/Unity/tree/master) and these results are inputted into Testrail with some custom additions to the Unity library to make this integration possible.

To be able to follow the README, you need to have a working Raspberyr Pi Zero 2 W with Debian (Raspbian) Lite 32-bit Bullseye installed with SSH enabled and a password of choice ocnfigured to login to the RPi.

## Dependencies install

### I²C
You will need to install some packages on your device for the I²C communication to work properly.
By typing the following commands in the terminal.
```
sudo apt-get install libi2c-dev
sudo apt-get install i2c-tools
```
Next you'll need to create a custom RAM drive to store te sensor data in a JSON file.
Just navigate to the root folder of your device and enter the following command to generate the directory.
```
sudo mkdir /var/data
```
While still in the root enter to edit the fstab file.
```
sudo nano /etc/fstab
```
Paste this line into fstab. And then save and close.
```
tmpfs /var/data tmpfs nodev,nosuid,size=1M 0 0 
```
Use the following command to enable the RAM drive.
```
sudo mount -a
```
You can check if the 1 Mb RAM drive has been succesfully created by typing ```df```, if a 1024 Kb drive shows up with the name /var/data, it has succesfuly been created and data can be written to it.

### MQTT
For the MQTT communication this project will use the Paho MQTT C client library from Eclipse.
To install the needed MQTT library you can follow the instructions on the [paho.mqtt.c](https://github.com/eclipse/paho.mqtt.c) repo.

### Curl (needed for implementation with Testrail)
To use Testrail as a tool for bundling all the testcases, curl has to be installed to do the https POST requests needed to communicate with the test suite.
For this just run ```sudo apt-get install libcurl4-openssl-dev``` in the root directory of the RPi.
This will intall curl internally.

## Connection sensor
To connect the BMP180 sensor to the Raspberry Pi:
- BMP180 ----- RPi
- Vin ----------- 3.3V
- GND --------- GND
- SDA ---------- SDA (GPIO 2)
- SCL ----------- SCL (GPIO 3)

## Generate TLS/SSL certificates
To get the required certificates run the following commands one by one.
After running the last command it will prompt you for a password, you can choose this password yourself but you have to remember it for later and enter it when prompted.
```
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365
cat key.pem cert.pem > combined.pem
openssl pkcs12 -export -in combined.pem -out keystore.p12 -name default -noiter -nomaciter
```
To generate the fingerprint ot input into the Azure Iot Hub run.
```
openssl x509 -in cert.pem -noout -fingerprint|tr -d ':'|cut -f2 -d=
```

## Building project
First you'll need to clone the repo by using git clone.
```
git clone https://github.com/DamianMoris0/StageOpdracht.git
```
To build application navigate into the cloned repo and type ```make```, this should succesfully build the application.
And finally execute the code by running ```./main.o```.
The application should now run and write the read data into a data.json file located in /var/data.

## Run on boot
If you want the code to run constantly without having to start the program manually you'll need to follow the follwing steps.
You do have to build the code manually once though, so that the deamon service knows which executable to run.
To start navigate to the /etc/systemd/system folder from your root directory, here you'll need to create a new .service file called "sensor-service.service" (make sure to use admin privileges for this).
In this file you should enter the following structure.
```
[Unit]
Description=Sensor Service

[Service]
Type=simple
RemainAfterExit=yes
ExecStart=/home/pi/Sensor/main.o
Restart=on-success
User=pi
WorkingDirectory=/home/pi/Sensor

[Install]
WantedBy=multi-user.target
```
Now run ```systemctl daemon-reload``` to reload the service.
If you run ```sudo systemctl enable sensor-service.service``` your code will now run at boot.
This can be disabled by running ```sudo systemctl disable sensor-service.service```.
