Index: readme.txt
===================================================================
--- readme.txt    (revision 202)
+++ readme.txt    (revision 203)
@@ -1,32 +1,32 @@
 
/====================
-管理器
     CsEntityManage
 
     Ctrl J
=================================================================
All possible values of HTTP Content-type header:

Type    Values
Application    application/EDI-X12
application/EDIFACT
application/javascript
application/octet-stream
application/ogg
application/pdf
application/xhtml+xml
application/x-shockwave-flash
application/json
application/ld+json
application/xml
application/zip
application/x-www-form-urlencoded
Audio    audio/mpeg
audio/x-ms-wma
audio/vnd.rn-realaudio
audio/x-wav
Image    image/gif
image/jpeg
image/png
image/tiff
image/vnd.microsoft.icon
image/x-icon
image/vnd.djvu
image/svg+xml
Multipart    multipart/mixed
multipart/alternative
multipart/related (using by MHTML (HTML mail).)
multipart/form-data
Text    text/css
text/csv
text/html
text/javascript (obsolete)
text/plain
text/xml
Video    video/mpeg
video/mp4
video/quicktime
video/x-ms-wmv
video/x-msvideo
video/x-flv
video/webm
VND    application/vnd.oasis.opendocument.text
application/vnd.oasis.opendocument.spreadsheet
application/vnd.oasis.opendocument.presentation
application/vnd.oasis.opendocument.graphics
application/vnd.ms-excel
application/vnd.openxmlformats-officedocument.spreadsheetml.sheet
application/vnd.ms-powerpoint
application/vnd.openxmlformats-officedocument.presentationml.presentation
application/msword
application/vnd.openxmlformats-officedocument.wordprocessingml.document
application/vnd.mozilla.xul+xml
==============================================================================

 
-物件
  _cstENTITYDESCRIPTOR
-    Entity 的描述性指標，可以把他視為一個 Entity的編號指標，但不是真正的 所以不能直接拿來存取
-    而要用該指標去詢問CsEntityManage
-    裡面才存放有真正的 Entity物件

  CsEntityDescriptor
-    讓DLL可以存取_cstENTITYDESCRIPTOR的物件，接收後會行將Entity的資料進行結束化的動作
 
 // 2008
 -預定要加入一個Map Server
-1.當GAMESERVER建立一個新的REGION(或是進入一個空的)時要告訴他
-2.當GAMESERVER的REGION空時時要告訴他
-3.當ENTITY更新位置的時候要告訴他 他再告訴擁有這些REGION的GAMESERVER 
-  要加入該SHADOW ENTITY或是 離開原本加入的SHADOW ENTITY

 
-50000 以上保留
command
#define CsID_DEMON  (_CsMAXCHANNEL+15100)

#define CsID_GAME   (_CsMAXCHANNEL+15200)
#define CsID_LOBBY  (_CsMAXCHANNEL+15300)
#define CsID_BASE   (_CsMAXCHANNEL+15400)
#define CsID_ARBIT  (_CsMAXCHANNEL+15500)

scs
#define CsID_SCS       (_CsMAXCHANNEL+15000)
#define CsSCSID_DEMON  (_CsMAXCHANNEL+15100)
#define CsSCSID_GAME   (_CsMAXCHANNEL+15200)


// ctrl + j 

//linux
cp    -ru $(RemoteProjectDir)/src/cs-def.h                   $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-err.h                   $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-obj_descriptor.h        $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-update_operation.h      $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-obj_descriptor.h        $(RemoteRootDir)/../include/net;
cp    -ru $(RemoteProjectDir)/src/cs-core_server.h           $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-http_server.h           $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-database_service.h      $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-app_server.h            $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-helper.h                $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-json.h                  $(RemoteRootDir)/../include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-entity_def.h            $(RemoteRootDir)/include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-protocol.h              $(RemoteRootDir)/include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-uid_generator.h         $(RemoteRootDir)/include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-arbitration_server.h    $(RemoteRootDir)/include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-record.h                $(RemoteRootDir)/include/net; 
cp    -ru $(RemoteProjectDir)/src/cs-paper_server.h          $(RemoteRootDir)/include/net;


// msvc
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
