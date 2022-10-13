// ctrl + j

// linux
cp    -ru $(RemoteProjectDir)/src/path_search.h     $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/ob_path_search.h  $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/cryptology.h      $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/crc32.h           $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/crc64.h           $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/base32.h          $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/base64.h          $(RemoteRootDir)/../include/algorithmic; 
cp    -ru $(RemoteProjectDir)/src/aes.h             $(RemoteRootDir)/../include/algorithmic;
cp    -ru $(RemoteProjectDir)/src/md5.h             $(RemoteRootDir)/../include/algorithmic;
cp    -ru $(RemoteProjectDir)/src/uri.h             $(RemoteRootDir)/../include/algorithmic;






copy /y src\path_search.h                           ..\..\include\algorithmic
copy /y src\ob_path_search.h                        ..\..\include\algorithmic
copy /y src\cryptology.h                            ..\..\include\algorithmic
copy /y src\crc32.h                                 ..\..\include\algorithmic
copy /y src\crc64.h                                 ..\..\include\algorithmic
copy /y src\base32.h                                ..\..\include\algorithmic
copy /y src\base64.h                                ..\..\include\algorithmic
copy /y src\aes.h                                   ..\..\include\algorithmic
copy /y src\md5.h                                   ..\..\include\algorithmic
copy /y src\uri.h                                   ..\..\include\algorithmic
      

//////
Crc32  Crc64  產生一個checksum的識別數字
自訂一個 crc 基本的數字
unsigned int Encode( unsigned int crc, const char* buffer, unsigned int buflen );
使用程式裡面自己建的
unsigned int Encode( const char* buffer, unsigned int buflen );


Base32 64
https://zh.wikipedia.org/wiki/Base64



