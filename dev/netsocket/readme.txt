// ctrl + j

// linux
cp    -ru $(RemoteProjectDir)/src/unp.h               $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/net-socket.h        $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/net-rudp_socket.h   $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/net-server.h        $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/net-client.h        $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/net-peer.h          $(RemoteRootDir)/../include/net;



copy /y     src\unp.h                                 ..\..\include\net
copy /y     src\net-socket.h                          ..\..\include\net
copy /y     src\net-server.h                          ..\..\include\net
copy /y     src\net-client.h                          ..\..\include\net
copy /y     src\net-peer.h                            ..\..\include\net