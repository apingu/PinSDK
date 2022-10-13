

1. example

	PCWin mywin;

	mywin.w32Create( "my", 640, 480, 16, false );
	mywin.SetIdleFunc( render );

	mywin.w32MainLoop();

	return;
