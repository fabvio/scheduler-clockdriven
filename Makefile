FLAGS = -DMULTIPROC -DTIMELOG

all: executive executive-err executive-err-b executive-err-ap

executive: executive.o task-ok.o busy_wait.o monitor.o
	gcc -o executive executive.o task-ok.o busy_wait.o monitor.o -lpthread

executive-err: executive.o task-error.o busy_wait.o monitor.o
	gcc -o executive-err executive.o task-error.o busy_wait.o monitor.o -lpthread

executive-err-b: executive.o task-err-b.o busy_wait.o monitor.o
	gcc -o executive-err-b executive.o task-err-b.o busy_wait.o monitor.o -lpthread

executive-err-ap: executive.o task-err-ap.o busy_wait.o monitor.o
	gcc -o executive-err-ap executive.o task-err-ap.o busy_wait.o monitor.o -lpthread

task-err-ap.o: task-err-ap.c
	gcc -c task-err-ap.c

task-err-b.o: task-err-b.c
	gcc -c task-err-b.c

task-error.o: task-error.c
	gcc -c task-error.c

monitor.o: monitor.c
	gcc -c monitor.c

executive.o: executive.c
	gcc -c executive.c $(FLAGS)

task-ok.o: task-ok.c
	gcc -c task-ok.c

busy_wait.o: busy_wait.c
	gcc -c busy_wait.c

clean:
	rm *.o executive executive-err executive-err-b executive-err-ap
