# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/unp.h               $1/../include/net #
cp    -ru $2/src/net-socket.h        $1/../include/net #
cp    -ru $2/src/net-rudp_socket.h   $1/../include/net #
cp    -ru $2/src/net-server.h        $1/../include/net #
cp    -ru $2/src/net-client.h        $1/../include/net #
cp    -ru $2/src/net-peer.h          $1/../include/net #