all: 
	gcc -Wall -Werror -pthread -o s-talk main.c list.c keyboard.c sender.c receiver.c screen.c

clean:
	rm s-talk