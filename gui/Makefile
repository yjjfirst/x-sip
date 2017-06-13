gui : gui.c ../sip/libsip.a ../utils/libutils.a
	$(CC) `pkg-config --cflags gtk+-3.0` -o gui  gui.c `pkg-config --libs gtk+-3.0` ../sip/libsip.a ../utils/libutils.a -I../include
clean :
	-@rm -f gui
