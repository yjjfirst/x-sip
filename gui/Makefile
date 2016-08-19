main : main.c
	$(CC) `pkg-config --cflags gtk+-3.0` -o main  main.c `pkg-config --libs gtk+-3.0` ../sip/libsip.a ../utils/libutils.a
clean :
	-@rm -f main
