# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
mkdir -p  $1/include #
mkdir -p  $1/include/net #
mkdir -p  $1/include/game #
mkdir -p  $1/include/helper #
mkdir -p  $1/include/graphic #
#
mkdir -p  $1/lib #
mkdir -p  $1/lib/windows_x86 #
mkdir -p  $1/lib/windows_x86/Debug #
mkdir -p  $1/lib/windows_x86/Release #
mkdir -p  $1/lib/windows_x64 #
mkdir -p  $1/lib/windows_x64/Debug #
mkdir -p  $1/lib/windows_x64/Release #
mkdir -p  $1/lib/linux_x64 #
mkdir -p  $1/lib/linux_x64/Debug #
mkdir -p  $1/lib/linux_x64/Release #
#
mkdir -p  $1/../include #
mkdir -p  $1/../include/net #
mkdir -p  $1/../include/game #
mkdir -p  $1/../include/game/ui #
mkdir -p  $1/../include/template #
mkdir -p  $1/../include/helper #
mkdir -p  $1/../include/parser #
mkdir -p  $1/../include/database #
mkdir -p  $1/../include/algorithmic #
mkdir -p  $1/../include/gadget #
mkdir -p  $1/../include/helper #
mkdir -p  $1/../include/SDK #
mkdir -p  $1/../include/SDK/D3D #
mkdir -p  $1/../include/SDK/SKY #
mkdir -p  $1/../include/SDK/RW #
#
mkdir -p  $1/../lib #
mkdir -p  $1/../lib/gc #
mkdir -p  $1/../lib/gc/Debug #
mkdir -p  $1/../lib/gc/Release #
mkdir -p  $1/../lib/ps2 #
mkdir -p  $1/../lib/ps2/Debug #
mkdir -p  $1/../lib/ps2/Release #
mkdir -p  $1/../lib/ps4 #
mkdir -p  $1/../lib/ps4/Debug #
mkdir -p  $1/../lib/ps4/Release #
mkdir -p  $1/../lib/xbox #
mkdir -p  $1/../lib/xbox/Debug #
mkdir -p  $1/../lib/xbox/Release #
mkdir -p  $1/../lib/ios #
mkdir -p  $1/../lib/ios/Debug #
mkdir -p  $1/../lib/ios/Release #
mkdir -p  $1/../lib/android #
mkdir -p  $1/../lib/android/Debug #
mkdir -p  $1/../lib/android/Release #
mkdir -p  $1/../lib/windows_x86 #
mkdir -p  $1/../lib/windows_x86/Debug #
mkdir -p  $1/../lib/windows_x86/Release #
mkdir -p  $1/../lib/windows_x64 #
mkdir -p  $1/../lib/windows_x64/Debug #
mkdir -p  $1/../lib/windows_x64/Release #
mkdir -p  $1/../lib/linux_x64 #
mkdir -p  $1/../lib/linux_x64/Debug #
mkdir -p  $1/../lib/linux_x64/Release #
#
cp    -ru $2/src/*.h            $1/../include #
cp    -ru $2/src/*.inl          $1/include #