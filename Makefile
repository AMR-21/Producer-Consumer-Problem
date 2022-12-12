CC = g++

all:
	$(CC) src/producer.cpp src/utility.cpp src/buffer.cpp -o producer 
	$(CC) src/consumer.cpp src/utility.cpp src/buffer.cpp src/conUtil.cpp -o consumer