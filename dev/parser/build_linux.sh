# $1 $(RemoteRootDir)       /root/projects/Engine/PinSDK/c/dev  
# $2 $(RemoteProjectDir)
echo $1
echo $2
#
cp    -ru $2/src/gsp/pgsp_file.h                 $1/../include/parser #
cp    -ru $2/src/xml/xmlschema.h                 $1/../include/parser #
cp    -ru $2/src/xml/xmlnode.h                   $1/../include/parser #
cp    -ru $2/src/json/json.h                     $1/../include/parser #
cp    -ru $2/src/json/json_value.h               $1/../include/parser #
cp    -ru $2/src/json/json_type.h                $1/../include/parser #
cp    -ru $2/src/configreader/config_reader.h    $1/../include/parser #
cp    -ru $2/src/http/mpfd-exception.h           $1/../include/parser #
cp    -ru $2/src/http/mpfd-field.h               $1/../include/parser #
cp    -ru $2/src/http/mpfd-parser.h              $1/../include/parser #
cp    -ru $2/src/http/http-parser.h              $1/../include/parser #
cp    -ru $2/src/html/html-parser.h              $1/../include/parser #