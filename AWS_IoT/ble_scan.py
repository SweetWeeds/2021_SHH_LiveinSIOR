import PyBeacon
#from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
# importing libraries
import paho.mqtt.client as paho
import os
import socket
import ssl
from time import sleep
from random import uniform

connflag = False

NameSpace = '01234567890123456789'
UserList = ['012345678901']

awshost = "athvs28tgejvh-ats.iot.ap-northeast-2.amazonaws.com"      # Endpoint
awsport = 8883                                              # Port no.   
clientId = "BUS_SYS1"                                       # Thing_Name
thingName = "BUS_SYS1"                                      # Thing_Name
caPath = "./AWS_IoT/root-CA.crt"                                      # Root_CA_Certificate_Name
certPath = "./AWS_IoT/BUS_SYS1.cert.pem"                            # <Thing_Name>.cert.pem
keyPath = "./AWS_IoT/BUS_SYS1.private.key"                          # <Thing_Name>.private.key

def on_connect(client, userdata, flags, rc):                # func for making connection
    global connflag
    print("Connected to AWS")
    connflag = True
    print("Connection returned result: " + str(rc) )
 
def on_message(client, userdata, msg):                      # Func for Sending msg
    print(msg.topic+" "+str(msg.payload))

def callback(client, userdata, message):
    print(message)

if __name__ == "__main__":
    mqttc = paho.Client()                                       # mqttc object
    mqttc.on_connect = on_connect                               # assign on_connect func
    mqttc.on_message = on_message                               # assign on_message func
    mqttc.tls_set(caPath, certfile=certPath, keyfile=keyPath, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)  # pass parameters
    mqttc.connect(awshost, awsport, keepalive=60)               # connect to aws server
    mqttc.loop_start()                                          # Start the loop
    
    while(True):
        scan_results = PyBeacon.scan(3)
        for s in scan_results:
            if s['type'] == 'uid':
                if s['content']['namespace'] == NameSpace and s['content']['instance'] in UserList:
                    mqttc.publish('passenger/get_in', '{"Handling":"True"}', qos=1)
            else:
                continue
