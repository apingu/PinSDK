// ctrl + j

// linux
cp    -ru $(RemoteProjectDir)/src/gsp/pgsp_file.h                 $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/xml/xmlschema.h                 $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/xml/xmlnode.h                   $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/json/json.h                     $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/json/json_value.h               $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/json/json_type.h                $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/configreader/config_reader.h    $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/http/mpfd-exception.h           $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/http/mpfd-field.h               $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/http/mpfd-parser.h              $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/http/http-parser.h              $(RemoteRootDir)/../include/parser;
cp    -ru $(RemoteProjectDir)/src/html/html-parser.h              $(RemoteRootDir)/../include/parser;

// windows
copy /y     src\gsp\pgsp_file.h                 ..\..\include\parser
copy /y     src\xml\xmlschema.h                 ..\..\include\parser
copy /y     src\xml\xmlnode.h                   ..\..\include\parser
copy /y     src\json\json.h                     ..\..\include\parser
copy /y     src\json\json_value.h               ..\..\include\parser
copy /y     src\json\json_type.h                ..\..\include\parser
copy /y     src\configreader\config_reader.h    ..\..\include\parser
copy /y     src\Http\http-parser.h              ..\..\include\parser
copy /y     src\Http\mpfd-field.h               ..\..\include\parser
copy /y     src\Http\mpfd-exception.h           ..\..\include\parser
copy /y     src\Http\mpfd-parser.h              ..\..\include\parser
copy /y     src\html\html-parser.h              ..\..\include\parser
