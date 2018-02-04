
all: 
		gcc -o par-naive par-naive.c -Wall -pthread 
		gcc -o par-optim par-optim.c -Wall -pthread 
		gcc -o seq seq.c -Wall -pthread

debug:
		gcc -o par-naive par-naive.c -Wall -pthread -pg -g
		gcc -o par-optim par-optim.c -Wall -pthread -pg -g
		gcc -o seq seq.c -Wall -pthread -pg -g

clean:
		rm -f par-optim seq par-naive	
