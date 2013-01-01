#ifdef _GUI_

#include "gtkui.h"

static void enter_callback( GtkWidget *widget,
                            GtkWidget *entry )
{
  const gchar *entry_text;
  entry_text = gtk_entry_get_text (GTK_ENTRY (entry));
  printf ("Entry contents: %s\n", entry_text);
}

static void entry_toggle_editable( GtkWidget *checkbutton,
                                   GtkWidget *entry )
{
  gtk_editable_set_editable (GTK_EDITABLE (entry),
                             GTK_TOGGLE_BUTTON (checkbutton)->active);
}

static void entry_toggle_visibility( GtkWidget *checkbutton,
                                     GtkWidget *entry )
{
  gtk_entry_set_visibility (GTK_ENTRY (entry),
			    GTK_TOGGLE_BUTTON (checkbutton)->active);
}

GtkWidget *FrontDownFrame(){
	GtkWidget *frame;
	frame=gtk_frame_new("Frontdown parameters:");
	
	GtkWidget *frameBox;
	frameBox=gtk_vbox_new(0, 5);



	GtkWidget *FrameSource;
	FrameSource=gtk_frame_new("Source:");

	GtkWidget *BoxSource;
	BoxSource=gtk_vbox_new(0,5);

	GtkWidget *BoxSourcePath;
	BoxSourcePath=gtk_hbox_new(0,5);

	GtkWidget *EntrySourcePath;
	EntrySourcePath=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY (EntrySourcePath), 16384);
	gtk_entry_set_text(GTK_ENTRY (EntrySourcePath), "./");
	
	GtkWidget *ButtonSourcePath;
	ButtonSourcePath=gtk_button_new();
	gtk_button_set_label(GTK_BUTTON (ButtonSourcePath), "Browse ...");



	GtkWidget *FrameDestination;
	FrameDestination=gtk_frame_new("Destination:");

	GtkWidget *BoxDestination;
	BoxDestination=gtk_vbox_new(0,5);

	GtkWidget *BoxDestinationPath;
	BoxDestinationPath=gtk_hbox_new(0,5);

	GtkWidget *DropDownDestinationPath;
	GList *DropDownDestinationList=NULL;
	DropDownDestinationPath=gtk_combo_new();
	gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (DropDownDestinationPath)->entry), "?????");
	DropDownDestinationList = g_list_append (DropDownDestinationList, "file://");
	DropDownDestinationList = g_list_append (DropDownDestinationList, "ftp://");
	gtk_combo_set_popdown_strings (GTK_COMBO (DropDownDestinationPath), DropDownDestinationList);

	GtkWidget *EntryDestinationPath;
	EntryDestinationPath=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY (EntryDestinationPath), 16384);
	gtk_entry_set_text(GTK_ENTRY (EntryDestinationPath), "ftp.example.com/MyBackup/");



	GtkWidget *FrameLogin;
	FrameLogin=gtk_frame_new("Login:");

	GtkWidget *BoxLogin;
	BoxLogin=gtk_hbox_new(0,5);

	GtkWidget *BoxLabel;
	BoxLabel=gtk_vbox_new(0,5);
	
	GtkWidget *LabelUser;
	LabelUser=gtk_label_new("User: ");

	GtkWidget *LabelPassword;
	LabelPassword=gtk_label_new("Password: ");

	GtkWidget *BoxEntry;
	BoxEntry=gtk_vbox_new(0,5);

	GtkWidget *EntryUser;
	EntryUser=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY (EntryUser), 256);
	gtk_entry_set_text(GTK_ENTRY (EntryUser), "");
	

	GtkWidget *EntryPassword;
	EntryPassword=gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY (EntryPassword), 256);
	gtk_entry_set_text(GTK_ENTRY (EntryPassword), "");
	gtk_entry_set_visibility(GTK_ENTRY (EntryPassword), 0);
	


    gtk_container_add (GTK_CONTAINER (frame), frameBox);

	//source
    gtk_box_pack_start (GTK_BOX (frameBox), FrameSource, 0, 1, 5);
    gtk_container_add  (GTK_CONTAINER (FrameSource), BoxSource);
    gtk_box_pack_start (GTK_BOX (BoxSource), BoxSourcePath, 1, 1, 5);
    gtk_box_pack_start (GTK_BOX (BoxSourcePath), EntrySourcePath, 1, 1, 5);
    gtk_box_pack_start (GTK_BOX (BoxSourcePath), ButtonSourcePath, 0, 1, 5);

	//destination
    gtk_box_pack_start (GTK_BOX (frameBox), FrameDestination, 0, 1, 5);
    gtk_container_add  (GTK_CONTAINER (FrameDestination), BoxDestination);
    gtk_box_pack_start (GTK_BOX (BoxDestination), BoxDestinationPath, 1, 1, 5);
    gtk_box_pack_start (GTK_BOX (BoxDestinationPath), DropDownDestinationPath, 0, 1, 5);
    gtk_box_pack_start (GTK_BOX (BoxDestinationPath), EntryDestinationPath, 1, 1, 5);

	//login
    gtk_box_pack_start (GTK_BOX (frameBox), FrameLogin, 0, 1, 5);
    gtk_container_add  (GTK_CONTAINER (FrameLogin), BoxLogin);
    gtk_box_pack_start (GTK_BOX (BoxLogin), BoxLabel, 0, 1, 5);	
    gtk_box_pack_start (GTK_BOX (BoxLabel), LabelUser, 0, 1, 10);	
    gtk_box_pack_start (GTK_BOX (BoxLabel), LabelPassword, 0, 1, 10);	
    gtk_box_pack_start (GTK_BOX (BoxLogin), BoxEntry, 1, 1, 5);	
    gtk_box_pack_start (GTK_BOX (BoxEntry), EntryUser, 1, 1, 5);	
    gtk_box_pack_start (GTK_BOX (BoxEntry), EntryPassword, 1, 1, 5);	
	
	return frame;
}

int gui(){
    GtkWidget *window;
    GtkWidget *main_box;

    gtk_init (0, NULL);

    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (window), 600, 500);
    gtk_window_set_title (GTK_WINDOW (window), "Frontdown GTK UI");
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

	main_box=gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (window), main_box);
    gtk_container_add (GTK_CONTAINER (main_box), FrontDownFrame());
    
    gtk_widget_show_all (window);


    gtk_main();

    return 0;
}

#endif
