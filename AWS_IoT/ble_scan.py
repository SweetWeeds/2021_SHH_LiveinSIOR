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

NameSpace = '7777772e73742e636f6d'
UserList = ['000000000001']

awshost = "a371nu4tbem1pv-ats.iot.ap-northeast-2.amazonaws.com"      # Endpoint
awsport = 8883                                              # Port no.   
clientId = "BUS_INFO_SYS1"                                       # Thing_Name
thingName = "BUS_INFO_SYS1"                                      # Thing_Name
caPath = "./root-CA.crt"                                      # Root_CA_Certificate_Name
certPath = "./BUS_INFO_SYS1.cert.pem"                           # <Thing_Name>.cert.pem
keyPath = "./BUS_INFO_SYS1.private.key"                          # <Thing_Name>.private.key

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
        scan_results = PyBeacon.scan(1)
        for s in scan_results:
            if s['type'] == 'uid':
                print("namespace:{}, instance:{}".format(s['content']['namespace'], s['content']['instance']))
                if s['content']['namespace'] == NameSpace and s['content']['instance'] in UserList:
                    print("Publishing...")
                    mqttc.publish('passenger/get_in', '{"ID":"%s", "Handling":"True"}' % s['content']['instance'], qos=1)
            else:
                continue
