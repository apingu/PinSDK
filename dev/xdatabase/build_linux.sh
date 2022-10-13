# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/xdatabase.h       $1/../include/database #
cp    -ru $2/src/xdb-helper.h      $1/../include/database #
cp    -ru $2/src/xdb-query.h       $1/../include/database #
cp    -ru $2/src/xdb-redisdb.h     $1/../include/database #
cp    -ru $2/src/xdb-uid_catalog.h $1/../include/database #