windows
  predefine    + WIN32_LEAN_AND_MEAN
  MQTTAsync.c DllMAin =>

  			if (mqttasync_mutex == NULL)
			{
				mqttasync_mutex = CreateMutex(NULL, 0, NULL);
				mqttcommand_mutex = CreateMutex(NULL, 0, NULL);
				send_sem = CreateEvent(
		        NULL,               /* default security attributes */
		        FALSE,              /* manual-reset event? */
		        FALSE,              /* initial state is nonsignaled */
		        NULL                /* object name */
		        );
				stack_mutex = CreateMutex(NULL, 0, NULL);
				heap_mutex = CreateMutex(NULL, 0, NULL);
				log_mutex = CreateMutex(NULL, 0, NULL);
				socket_mutex = CreateMutex(NULL, 0, NULL);
			}
		加到  MQTTAsync_create  因為 原本會再Dll 開始時呼叫 但現在不用dll的方式

		#if defined(WIN32) || defined(WIN64)
            if( mqttasync_mutex == NULL )
            {
                mqttasync_mutex = CreateMutex( NULL, 0, NULL );
                mqttcommand_mutex = CreateMutex( NULL, 0, NULL );
                send_sem = CreateEvent(
                    NULL,               /* default security attributes */
                    FALSE,              /* manual-reset event? */
                    FALSE,              /* initial state is nonsignaled */
                    NULL                /* object name */
                );
                stack_mutex = CreateMutex( NULL, 0, NULL );
                heap_mutex = CreateMutex( NULL, 0, NULL );
                log_mutex = CreateMutex( NULL, 0, NULL );
                socket_mutex = CreateMutex( NULL, 0, NULL );
            }
        #endif

linux
    predefin + _XOPEN_SOURCE   防止  struct sigaction sa; 的問題
               #define _BSD_SOURCE  防止 undefined reference tohtole32' 16 64
    thread.c 加上  #define CLOCK_REALTIME 0 