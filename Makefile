sorter_client:
	gcc -pthread -o sorter_client sorter_client.c client_helpers.c
sorter_server:
	gcc -pthread -o sorter_server sorter_server.c mergesort.c
clean:
	rm -f sorter_client
	rm -f sorter_client.o
	rm -f sorter_server
	rm -f sorter_server.o
