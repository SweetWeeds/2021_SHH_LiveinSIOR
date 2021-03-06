import paho.mqtt.client as paho
import os
import socket
import ssl
import json
import lcddriver
import pygame
import time


CHUNK = 1024
targetTopic = 'passenger/get_in'
NameSpace = '7777772e73742e636f6d'
UserList = ['000000000001']
mutex = 0

lcd = lcddriver.lcd()
lcd.lcd_clear()
lcd.lcd_display_string("7800")

def play_audio(filename):
    freq = 24000    # sampling rate, 44100(CD), 16000(Naver TTS), 24000(google TTS)
    bitsize = -16   # signed 16 bit. support 8,-8,16,-16
    channels = 1    # 1 is mono, 2 is stereo
    buffer = 2048   # number of samples (experiment to get right sound)

    # default : pygame.mixer.init(frequency=22050, size=-16, channels=2, buffer=4096)
    pygame.mixer.init(freq, bitsize, channels, buffer)
    pygame.mixer.music.load(filename)
    pygame.mixer.music.play()
    clock = pygame.time.Clock()
    while pygame.mixer.music.get_busy():
        clock.tick(3)
    pygame.mixer.quit()

def on_connect(client, userdata, flags, rc):                # func for making connection
    print("Connection returned result: " + str(rc) )
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("#" , 1 )                              # Subscribe to all topics
 
def on_message(client, userdata, msg):                      # Func for receiving msgs
    global mutex    # 뮤텍스 구현
    if (mutex == 0):
        mutex = 1
        # 수신 데이터 출력
        print("topic: "+msg.topic)
        print("payload: "+str(msg.payload.decode("utf-8")))
        if (msg.topic == targetTopic):
            payload = json.loads(str(msg.payload.decode("utf-8")))
            if (payload['Handling'] == 'True'):
                # LCD 출력
                lcd.lcd_clear()
                lcd.lcd_display_string("Need a help", 1)
                lcd.lcd_display_string("At next stop", 2)
                # 경고 출력
                play_audio("./output.mp3")
                # LCD 초기화
                lcd.lcd_clear()
                lcd.lcd_display_string("7800")
        mutex = 0
    else:
        pass

    
#def on_log(client, userdata, level, msg):
#    print(msg.topic+" "+str(msg.payload))
 
mqttc = paho.Client()                                       # mqttc object
mqttc.on_connect = on_connect                               # assign on_connect func
mqttc.on_message = on_message                               # assign on_message func
#mqttc.on_log = on_log

#### Change following parameters ####  
awshost = "a371nu4tbem1pv-ats.iot.ap-northeast-2.amazonaws.com"      # Endpoint
awsport = 8883                                              # Port no.   
clientId = "BUS1"                                       # Thing_Name
thingName = "BUS1"                                      # Thing_Name
caPath = "./root-CA.crt"                                      # Root_CA_Certificate_Name
certPath = "./BUS1.cert.pem"                            # <Thing_Name>.cert.pem
keyPath = "./BUS1.private.key"                          # <Thing_Name>.private.key
 
mqttc.tls_set(caPath, certfile=certPath, keyfile=keyPath, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)      # pass parameters
 
mqttc.connect(awshost, awsport, keepalive=60)               # connect to aws server
 
mqttc.loop_forever()                                        # Start receiving in loop
