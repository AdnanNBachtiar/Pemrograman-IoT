import paho.mqtt.client as mqtt

client = mqtt.Client()

client.connect("broker.emqx.io", 1883, 60)


while True:
    payload = input("Masukan Pesan: ")

    print("Publishing Message....")
    client.publish("IoT_Command", payload)
    print("Message Published\n")