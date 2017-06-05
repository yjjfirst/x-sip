#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <string.h>

#include "Init.h"
#include "Udp.h"
#include "CallEvents.h"

static int sockfd;
struct ClientMessage CurrEvent;

static void send_ringing()
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    struct ClientMessage event;

    event.ua = CurrEvent.ua;
    event.event = RINGING;
    BuildClientMessage(msg, event.ua, event.event, NULL);

    UdpSend(msg, "192.168.10.1", 5555,  sockfd);

}

static void send_accept (GtkWidget *widget, gpointer data)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    struct ClientMessage event;

    event.ua = CurrEvent.ua;
    event.event = ACCEPT_CALL;
    BuildClientMessage(msg, event.ua, event.event, NULL);

    UdpSend(msg, "192.168.10.1", 5555,  sockfd);
}

static void send_callout (GtkWidget *widget, gpointer data)
{
    char msg[CLIENT_MESSAGE_MAX_LENGTH] = {0};
    struct ClientMessage event;

    event.ua = CurrEvent.ua;
    event.event = MAKE_CALL;
    strcpy(event.data,"88005");
    BuildClientMessage(msg, event.ua, event.event, event.data);

    UdpSend(msg, "192.168.10.1", 5555,  sockfd);
}


static void add_all_widget(GtkWidget *window)
{
    GtkWidget *make_call_button;
    GtkWidget *number;
    GtkWidget *status_label;
    GtkWidget *digits_grid_box;
    GtkWidget *vbox;
    GtkWidget *button;

    
    make_call_button = gtk_button_new_with_label ("Make Call");
    g_signal_connect_swapped (make_call_button, "clicked", G_CALLBACK (send_callout), window);
    
    number = gtk_entry_new();
    vbox =  gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);

    status_label = gtk_label_new("Status:");
    gtk_label_set_xalign((GtkLabel *)status_label, 0);

    digits_grid_box = gtk_grid_new();
    
    button = gtk_button_new_with_label ("1");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 0, 0, 1, 1);
    button = gtk_button_new_with_label ("2");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 1, 0, 1, 1);    
    button = gtk_button_new_with_label ("3");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 2, 0, 1, 1);    

    button = gtk_button_new_with_label ("4");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 0, 1, 1, 1);
    button = gtk_button_new_with_label ("5");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 1, 1, 1, 1);    
    button = gtk_button_new_with_label ("6");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 2, 1, 1, 1);    

    button = gtk_button_new_with_label ("7");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 0, 2, 1, 1);
    button = gtk_button_new_with_label ("8");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 1, 2, 1, 1);    
    button = gtk_button_new_with_label ("9");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 2, 2, 1, 1);    

    button = gtk_button_new_with_label ("*");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 0, 3, 1, 1);
    button = gtk_button_new_with_label ("0");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 1, 3, 1, 1);    
    button = gtk_button_new_with_label ("#");
    gtk_grid_attach (GTK_GRID (digits_grid_box), button, 2, 3, 1, 1);    

    
    gtk_box_pack_start(GTK_BOX (vbox), number, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX (vbox), status_label, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX (vbox), digits_grid_box, TRUE, TRUE, 1);
    gtk_box_pack_end(GTK_BOX(vbox), make_call_button, FALSE, FALSE, 1);

    gtk_container_add(GTK_CONTAINER(window), vbox);    
}

static void activate (GtkApplication* app, gpointer user_data)
{
    GtkWidget *window;
    
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "X-SIP");
    gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);

    add_all_widget(window);
    
    gtk_widget_show_all (window);
}

gboolean sock_callback(GIOChannel *source, GIOCondition condition, gpointer data)
{
    char *buf;
    gsize length;
    
    g_io_channel_read_line(source, &buf, &length, NULL, NULL);

    ParseClientMessage(buf, &CurrEvent);
    if (CurrEvent.event == CALL_INCOMING)
        send_ringing();
    return TRUE;
}

int main (int argc, char **argv)
{
    GtkApplication *app;
    int status;
    GIOChannel *io_channel = NULL;

    sockfd = UdpInit(5556);

    io_channel = g_io_channel_unix_new (sockfd);    
    g_io_channel_set_encoding (io_channel, NULL, NULL);
    g_io_add_watch (io_channel, G_IO_IN, sock_callback, NULL);
    
    InitStack();
    
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
