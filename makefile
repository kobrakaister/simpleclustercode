CC=gcc
OBJS=node.o head.o util.o inp.o rx_file.c tx_file.c jobs.o nodes.o command.o rx_loop.o cal_path.o run_jobs.o ip.o register.o killall.o sleep.o poweroff.o get_data.o clean.o progress.o load.o quit.o encrypt.o sync.o tx_packet.o

main: main.c $(OBJS)
	gcc main.c $(OBJS) -o cluster.o -lzip -lpthread -lssl -lcrypto

.c.o : 
	$(CC) -c $< -o $@

clean:
	rm *.o -f
