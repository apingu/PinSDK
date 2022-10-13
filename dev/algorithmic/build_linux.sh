# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/path_search.h     $1/../include/algorithmic # 
cp    -ru $2/src/ob_path_search.h  $1/../include/algorithmic # 
cp    -ru $2/src/cryptology.h      $1/../include/algorithmic # 
cp    -ru $2/src/crypt-crc32.h     $1/../include/algorithmic # 
cp    -ru $2/src/crypt-crc64.h     $1/../include/algorithmic # 
cp    -ru $2/src/crypt-base32.h    $1/../include/algorithmic # 
cp    -ru $2/src/crypt-base64.h    $1/../include/algorithmic # 
cp    -ru $2/src/crypt-aes.h       $1/../include/algorithmic # 
cp    -ru $2/src/crypt-md5.h       $1/../include/algorithmic # 
cp    -ru $2/src/uri.h             $1/../include/algorithmic # 