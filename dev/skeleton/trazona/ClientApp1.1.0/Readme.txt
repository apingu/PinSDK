





                                    LoginServer()
									      |
										  |
										  |
										  |
										  |
										  |
             --------------------------------------------------------------
             |                            |                                |
			 |                            |                                |
       CreateCharacter()                  |                         DeleteCharacter()
	         |                            |                                |
	         |                            |                                |
			 -----------------------------|--------------------------------
										  |
										  |
                                eid = SelectCharacter()
								          |
										  |
										  |
										  |
										  |
                                  RegisterCallback();
  								          |
										  |
										  |
										  |
										  |
                                 EntityJoinGame( eid );
								          |
										  |
										  |
										  |
										  |

                                    idle
                                    {
                                        SendStats();
                                    }
