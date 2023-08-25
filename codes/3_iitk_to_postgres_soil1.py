# PROGRAM OF RECEIVING DATA FROM MTQQ AND UPLOADING DATA PGADMIN
import codecs

import paho.mqtt.subscribe as subscribe
# importing psycopg2 module
import psycopg2

topic = ["outTopic_22103052"]  # TOPIC NAME FROM WE GET DATA
hostname_1 = "172.26.214.245"  # BROKER ID #broker.mqtt-dashboard.com
port_1 = 1883
username_1 = ""
password_1 = ""
# establishing the connection
conn = psycopg2.connect(
    database="postgres",
    user='postgres',
    password='postgres',
    host='localhost',
    port='5432'
)
# creating a cursor object
cursor = conn.cursor()
# creating table
sql = '''CREATE TABLE soil(
id SERIAL NOT NULL,
date varchar(20) not null,
time varchar(20) not null,
vmc varchar(40) not null,
Un_vmc varchar(40) not null,
TEMPERATURE varchar(20) not null,
HUMIDITY varchar(20) not null
)'''
# cursor.execute(sql)
while True:
    message = subscribe.simple(topic, hostname=hostname_1, port=port_1, keepalive=60, will=None, )
    # auth={'username':username_1,'password':password_1})
    print(message.payload.decode())  # PRINTING THE MESSAGE OF HUMIDITY AND TEMPERATURE
    print(message.topic)  # PRINTING THE TOPIC NAME FROM WHERE WE GET DATA
    # cursor.execute(sql)
    data_sensor = str(codecs.decode(message.payload))
    print("data_sensor", data_sensor)
    sql1 = "INSERT INTO soil (date,time,vmc,Un_vmc, TEMPERATURE, HUMIDITY) VALUES (%s,%s,%s,%s,%s,%s)"
    data = data_sensor.split(",")
    val = (data[0], data[1], data[2], data[3], data[5], data[4])
    print(val)
    cursor.execute(sql1, val)
    print("List has been inserted to soil table successfully...")
    # Commit your changes in the database
    conn.commit()
    # Closing the connection
    # conn.close()
