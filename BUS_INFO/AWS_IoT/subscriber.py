import paho.mqtt.client as paho
import os
import socket
import ssl
 
def on_connect(client, userdata, flags, rc):                # func for making connection
    print("Connection returned result: " + str(rc) )
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("#" , 1 )                              # Subscribe to all topics
 
def on_message(client, userdata, msg):                      # Func for receiving msgs
    print("topic: "+msg.topic)
    print("payload: "+str(msg.payload))
     
#def on_log(client, userdata, level, msg):
#    print(msg.topic+" "+str(msg.payload))
 
mqttc = paho.Client()                                       # mqttc object
mqttc.on_connect = on_connect                               # assign on_connect func
mqttc.on_message = on_message                               # assign on_message func
#mqttc.on_log = on_log

#### Change following parameters ####  
awshost = "athvs28tgejvh-ats.iot.ap-northeast-2.amazonaws.com"      # Endpoint
awsport = 8883                                              # Port no.   
clientId = "BUS_SYS1"                                       # Thing_Name
thingName = "BUS_SYS1"                                      # Thing_Name
caPath = "./root-CA.crt"                                      # Root_CA_Certificate_Name
certPath = "./BUS_SYS1.cert.pem"                            # <Thing_Name>.cert.pem
keyPath = "./BUS_SYS1.private.key"                          # <Thing_Name>.private.key
 
mqttc.tls_set(caPath, certfile=certPath, keyfile=keyPath, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)      # pass parameters
 
mqttc.connect(awshost, awsport, keepalive=60)               # connect to aws server
 
mqttc.loop_forever()                                        # Start receiving in loop
