# $1 /root/projects/Engine/PinSDK/c/dev/lib/linux_x64/Debug $(RemoteRootDir)/lib/
# $2 $(Platform)/$(Configuration)/
# $3 libsdkserver                                     $(TargetName)
cd $1/lib/$2
find . -name '*.a' -exec ar -x {} \;
ar cru $1/../lib/$2/$3.a *.o
ranlib $1/../lib/$2/$3.a
#ar rc $1/../lib/$2/$3.a crc32.o
#rm -rf *.o
echo "list directory"
ls
echo "list $1/../lib/$2/"
ls $1/../lib/$2/