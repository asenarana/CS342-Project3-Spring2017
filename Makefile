default:
	gcc -pthread -g -Wall -o phil phil.c -lm
clean:
	rm -f phil
