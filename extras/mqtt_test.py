from paho.mqtt import client as mqtt
import ssl

#path_to_root_cert = "../../Certificates/cert.pem"
cert_file = "../../Certificates/cert.pem"
key_file = "../../Certificates/key_nopass.pem"
device_id = "clientId-test-stage-example"
#iot_hub_name = "IotHubStage"
#client_username = iot_hub_name + ".azure-devices.net/" + device_id + "/?api-version=2021-04-12"
user = "demouser"

def on_connect(client, userdata, flags, rc):
    print("Device connected with result code: " + str(rc))


def on_disconnect(client, userdata, rc):
    print("Device disconnected with result code: " + str(rc))


def on_publish(client, userdata, mid):
    print("Device sent message")


client = mqtt.Client(client_id=device_id, protocol=mqtt.MQTTv311) #, transport='websockets')

client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

# Set the username but not the password on your client
client.username_pw_set(username=None, password=None)

#client.tls_set(ca_certs=None, certfile=cert_file, keyfile=key_file, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)

client.connect("172.16.0.72", port=1883)
#client.connect(iot_hub_name+".azure-devices.net", port=8883)

client.publish("sensor/data", '{"temperature": 20.0, "pressure": 1000.0}', qos=1)
client.loop_forever()
