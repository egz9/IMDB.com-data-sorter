sorter_client:
	gcc -pthread -o sorter_client sorter_client.c sortcsv.c mergesort.c
sorter_thread:
	gcc -pthread -o sorter_thread sorter_thread.c mergesort.c sortcsv.c
DBSorter_thread:
	gcc -pthread -g -o DBSorter sorter_thread.c mergesort.c sortcsv.c
sortcsv:
	gcc -o sortcsv sortcsv.c mergesort.c
clean:
	rm -f sorter_thread
	rm -f sorter_thread.o
	rm -f sorter_client
	rm -f sorter_client.o
	rm -f sortcsv
	rm -f sortcsv.o
	rm -f DBSorter
	rm -f DBSorter.o
