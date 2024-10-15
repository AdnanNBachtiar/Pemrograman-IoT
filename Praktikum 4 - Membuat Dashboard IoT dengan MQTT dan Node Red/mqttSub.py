import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("IoT_Sensor")

def on_message(client, userdata, message):
    hasil = message.payload.decode('UTF-8')
    print("Pesan :", hasil)

client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message
client.connect("broker.emqx.io", 1883, 60)

client.loop_forever()