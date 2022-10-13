Ctrl+J

// linux
mkdir -p  $(RemoteRootDir)/../dev/include; 
mkdir -p  $(RemoteRootDir)/../dev/include/net; 
mkdir -p  $(RemoteRootDir)/../dev/include/game; 
mkdir -p  $(RemoteRootDir)/../dev/include/helper; 
mkdir -p  $(RemoteRootDir)/../dev/include/graphic; 

mkdir -p  $(RemoteRootDir)/../dev/lib; 
mkdir -p  $(RemoteRootDir)/../dev/lib/Win32; 
mkdir -p  $(RemoteRootDir)/../dev/lib/Win32/Debug; 
mkdir -p  $(RemoteRootDir)/../dev/lib/Win32/Release; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x86; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x86/Debug; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x86/Release; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x64; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x64/Debug; 
mkdir -p  $(RemoteRootDir)/../dev/lib/x64/Release; 

mkdir -p  $(RemoteRootDir)/../include; 
mkdir -p  $(RemoteRootDir)/../include/net; 
mkdir -p  $(RemoteRootDir)/../include/game; 
mkdir -p  $(RemoteRootDir)/../include/game/ui; 
mkdir -p  $(RemoteRootDir)/../include/template; 
mkdir -p  $(RemoteRootDir)/../include/helper; 
mkdir -p  $(RemoteRootDir)/../include/parser; 
mkdir -p  $(RemoteRootDir)/../include/database; 
mkdir -p  $(RemoteRootDir)/../include/algorithmic; 
mkdir -p  $(RemoteRootDir)/../include/SDK; 
mkdir -p  $(RemoteRootDir)/../include/SDK/D3D; 
mkdir -p  $(RemoteRootDir)/../include/SDK/SKY; 
mkdir -p  $(RemoteRootDir)/../include/SDK/RW; 

mkdir -p  $(RemoteRootDir)/../lib; 
mkdir -p  $(RemoteRootDir)/../lib/GC; 
mkdir -p  $(RemoteRootDir)/../lib/GC/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/GC/Release; 
mkdir -p  $(RemoteRootDir)/../lib/PS2; 
mkdir -p  $(RemoteRootDir)/../lib/PS2/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/PS2/Release; 
mkdir -p  $(RemoteRootDir)/../lib/iOS; 
mkdir -p  $(RemoteRootDir)/../lib/iOS/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/iOS/Release; 
mkdir -p  $(RemoteRootDir)/../lib/Win32; 
mkdir -p  $(RemoteRootDir)/../lib/Win32/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/Win32/Release; 
mkdir -p  $(RemoteRootDir)/../lib/XBox; 
mkdir -p  $(RemoteRootDir)/../lib/XBox/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/XBox/Release; 
mkdir -p  $(RemoteRootDir)/../lib/Linux; 
mkdir -p  $(RemoteRootDir)/../lib/Linux/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/Linux/Release; 
mkdir -p  $(RemoteRootDir)/../lib/x64; 
mkdir -p  $(RemoteRootDir)/../lib/x64/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/x64/Release; 
mkdir -p  $(RemoteRootDir)/../lib/Android; 
mkdir -p  $(RemoteRootDir)/../lib/Android/Debug; 
mkdir -p  $(RemoteRootDir)/../lib/Android/Release; 

cp    -ru $(RemoteProjectDir)/src/*.h            $(RemoteRootDir)/../include; 
cp    -ru $(RemoteProjectDir)/src/*.inl          $(RemoteRootDir)/include; 


// msvs
mkdir -p  $(RemoteRootDir)\..\dev\include\net; 
mkdir -p  $(RemoteRootDir)\..\dev\include\game; 
mkdir -p  $(RemoteRootDir)\..\dev\include\helper; 
mkdir -p  $(RemoteRootDir)\..\dev\include\graphic; 
mkdir -p  $(RemoteRootDir)\..\dev\lib; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x86; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x86\Debug; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x86\Release; 
mkdir -p  $(RemoteRootDir)\..\dev\lib; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x64; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x64\Debug; 
mkdir -p  $(RemoteRootDir)\..\dev\lib\x64\Release; 
mkdir -p  $(RemoteRootDir)\..\include; 
mkdir -p  $(RemoteRootDir)\..\include\net; 
mkdir -p  $(RemoteRootDir)\..\include\game; 
mkdir -p  $(RemoteRootDir)\..\include\game\ui; 
mkdir -p  $(RemoteRootDir)\..\include\template; 
mkdir -p  $(RemoteRootDir)\..\include\helper; 
mkdir -p  $(RemoteRootDir)\..\include\parser; 
mkdir -p  $(RemoteRootDir)\..\include\database; 
mkdir -p  $(RemoteRootDir)\..\include\algorithmic; 
mkdir -p  $(RemoteRootDir)\..\include\SDK; 
mkdir -p  $(RemoteRootDir)\..\include\SDK\D3D; 
mkdir -p  $(RemoteRootDir)\..\include\SDK\SKY; 
mkdir -p  $(RemoteRootDir)\..\include\SDK\RW; 
mkdir -p  $(RemoteRootDir)\..\lib; 
mkdir -p  $(RemoteRootDir)\..\lib\x86; 
mkdir -p  $(RemoteRootDir)\..\lib\x86\Debug; 
mkdir -p  $(RemoteRootDir)\..\lib\x86\Release; 
mkdir -p  $(RemoteRootDir)\..\lib\x64; 
mkdir -p  $(RemoteRootDir)\..\lib\x64\Debug; 
mkdir -p  $(RemoteRootDir)\..\lib\x64\Release; 

copy /y  src\*.h             ..\..\include
copy /y  src\*.inl           ..\include

$(ProjectDir)bin\$(Platform)\$(Configuration)\


=================================================================================
escape string
https://en.wikipedia.org/wiki/Escape_sequences_in_C
Escape sequence     Hex value in ASCII	Character represented
\a	                07	                Alert (Beep, Bell) (added in C89)[1]
\b      	        08                  Backspace
\e         (note1)  1B	                Escape character
\f	                0C	                Formfeed Page Break
\n	                0A	                Newline (Line Feed); see notes below
\r	                0D	                Carriage Return
\t	                09	                Horizontal Tab
\v	                0B	                Vertical Tab
\\	                5C	                Backslash
\'	                27	                Apostrophe or single quotation mark
\"	                22	                Double quotation mark
\?	                3F	                Question mark (used to avoid trigraphs)
\nnn       (note2)  any	                The byte whose numerical value is given by nnn interpreted as an octal number
\xhh…	            any	                The byte whose numerical value is given by hh… interpreted as a hexadecimal number
\uhhhh     (note3)  none                Unicode code point below 10000 hexadecimal (added in C99)[1]: 26 
\Uhhhhhhhh (note4)	none                Unicode code point where h is a hexadecimal digit

Note 1.^ Common non-standard code; see the Notes section below.
Note 2.^ There may be one, two, or three octal numerals n present; see the Notes section below.
Note 3.^ \u takes 4 hexadecimal digits h; see the Notes section below.
Note 4.^ \U takes 8 hexadecimal digits h; see the Notes section below.