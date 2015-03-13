all: listener

text: talker listener

talker: talker/talker.cpp talker/talker.h talker/main.cpp config.h
	g++ -g -Wall -o talker.bin main.cpp

listener: listener.cpp listener.h main.cpp ../config.h
	g++ -g -Wall -o listener.bin main.cpp





clean:
	$(RM) talker.bin
	$(RM) listener.bin
