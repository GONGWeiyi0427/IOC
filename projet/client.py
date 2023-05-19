import paho.mqtt.client as mqttClient

# Callback function to handle incoming MQTT messages and write them to a text file
def on_message(client, userdata, message):
    with open('mqtt_lum.txt', 'a') as file:
        file.write(f"Topic: {message.topic}\n")
        file.write(f"Message: {message.payload.decode()}\n\n")

# Create an MQTT client instance and set the callback function
client = mqttClient.Client()

# Set the username and password for the MQTT broker
broker_username = "gong"
broker_password = "weiyigong"
client.username_pw_set(broker_username, broker_password)

client.on_message = on_message

# Connect to the MQTT broker and subscribe to the desired topic(s)
broker_address = "172.20.10.8"  # Replace with your broker address
topic = "topic_lum"  # Replace with the topic you want to subscribe to

client.connect(broker_address)
client.subscribe(topic)
client.loop_start()

try:
    # Run the MQTT client and wait for incoming messages
    client.loop_forever()
except KeyboardInterrupt:
    # Disconnect the MQTT client on program termination
    client.disconnect()
    client.loop_stop()
