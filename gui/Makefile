main : main.c ../sip/libsip.a ../utils/libutils.a
	$(CC) `pkg-config --cflags gtk+-3.0` -o main  main.c `pkg-config --libs gtk+-3.0` ../sip/libsip.a ../utils/libutils.a -I../include
clean :
	-@rm -f main
