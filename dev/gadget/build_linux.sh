# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/*.h         $1/../include/gadget #