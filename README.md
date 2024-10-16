# Stage Opdracht

## Intro
In this repo you will find code to read temperature and pressure data from a BMP180 sensor with a Raspberry Pi Zero 2 W and save it in RAM memory in a JSON file.

## Dependencies install
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

## Connection sensor
To connect the BMP180 sensor to the Raspberry Pi:
- BMP180 ----- RPi
- Vin ----------- 3.3V
- GND --------- GND
- SDA ---------- SDA (GPIO 2)
- SCL ----------- SCL (GPIO 3)

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
