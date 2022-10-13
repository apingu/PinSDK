// generate BIG5 characters list
#include <stdio.h>

#define MAX_COLUMN 32

void main()
   {
   FILE *fp;
   unsigned char hb, lb;
   int counter;

   fp = fopen( "big5.lst", "wb" );

   counter = 0;
   for ( hb = 0xa1; hb <= 0xc6; hb++ )
      for ( lb = 0x40; lb <= 0xfe; lb++ )
         {
         if ( lb == 0x7f )
            lb = 0xa1;
         fputc( hb, fp );
         fputc( lb, fp );
         counter++;
         if ( counter == MAX_COLUMN )
            {
            counter = 0;
            fputc( '\n', fp );
            }
         }
   fclose( fp );
   }
