# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/cs-def.h                   $1/../include/net #
cp    -ru $2/src/cs-err.h                   $1/../include/net #
cp    -ru $2/src/cs-obj_descriptor.h        $1/../include/net #
cp    -ru $2/src/cs-database_service.h      $1/../include/net #
cp    -ru $2/src/cs-core_server.h           $1/../include/net #
cp    -ru $2/src/cs-http_server.h           $1/../include/net #
cp    -ru $2/src/cs-helper.h                $1/../include/net #
cp    -ru $2/src/cs-json.h                  $1/../include/net #
cp    -ru $2/src/cs-entity_def.h            $1/include/net # 
cp    -ru $2/src/cs-protocol.h              $1/include/net # 
cp    -ru $2/src/cs-uid_generator.h         $1/include/net # 
cp    -ru $2/src/cs-arbitration_server.h    $1/include/net # 
cp    -ru $2/src/cs-record.h                $1/include/net # 
cp    -ru $2/src/cs-paper_server.h          $1/include/net # 