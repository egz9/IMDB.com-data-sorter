sorter:
	gcc -pthread -o sorter sorter_thread.c mergesort.c sortcsv.c
DBSorter:
	gcc -pthread -g -o DBSorter sorter_thread.c mergesort.c sortcsv.c
sortcsv:
	gcc -o sortcsv sortcsv.c mergesort.c
clean:
	rm -f sorter
	rm -f sorter.o
	rm -f sortcsv
	rm -f sortcsv.o
	rm -f DBSorter
	rm -f DBSorter.o
