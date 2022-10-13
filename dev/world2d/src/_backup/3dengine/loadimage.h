//LoadImage.h
//
//    load image function
//
//
//
//
//
//
//
//
//
//
//                                               Copyright    (C)    2000    Pin
//

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "..//Base//tImage.h"
#include "..//Image//Loadbmp.h"
//#include "..//Conversion//jpeglib.h"



//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline tImage* LoadBMPTexture( const char* filename )
{
    FILE* f;
    BYTE* lpImage;

    if( ( f = fopen( filename, "rb" ) ) == NULL )
        return false;

    if( LoadBMP( f, &lpImage, PIXELFMT_RGBA ) != BMPERR_OK )
        return 0;

    tImage* ImageRec= new tImage;

    ImageRec->width = *( WORD * ) ( lpImage );
    ImageRec->height = *( WORD * ) ( lpImage + 2 );
    ImageRec->pixels = new BYTE[ ImageRec->width * ImageRec->height ];

    memcpy( ImageRec->pixels,
            ( lpImage + 4 ),
            ( ImageRec->width * ImageRec->height ) );

    if( lpImage != 0 )
        delete [] lpImage;

    ImageRec->width = ImageRec->width / 4 ;

    return ImageRec;
}

/*
//=============================================================================
//  Name : DecodeJPG
//  Desc : This decodes the jpeg and fills in the tImage structure
//=============================================================================

inline void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData)
{
    // Read in the header of the jpeg file
    jpeg_read_header(cinfo, TRUE);
    
    // Start to decompress the jpeg file with our compression info
    jpeg_start_decompress(cinfo);

    // Get the image dimensions and channels to read in the pixel data
    pImageData->channels = cinfo->num_components;
    pImageData->sizeX    = cinfo->image_width;
    pImageData->sizeY    = cinfo->image_height;

    // Get the row span in bytes for each row
    int rowSpan = cinfo->image_width * cinfo->num_components;
    
    // Allocate memory for the pixel buffer
    pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*rowSpan*pImageData->sizeY));
            
    // Create an array of row pointers
    unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];

    for (int i = 0; i < pImageData->sizeY; i++)
        rowPtr[i] = &(pImageData->data[i * rowSpan]);

    // Now comes the juice of our work, here we extract all the pixel data
    int rowsRead = 0;
    while (cinfo->output_scanline < cinfo->output_height) 
    {
        // Read in the current row of pixels and increase the rowsRead count
        rowsRead += jpeg_read_scanlines(cinfo, 
                                        &rowPtr[rowsRead], cinfo->output_height - rowsRead);
    }
    
    // Delete the temporary row pointers
    delete [] rowPtr;

    // Finish decompressing the data
    jpeg_finish_decompress(cinfo);
}


//=============================================================================
//  Name : LoadJPG
//  Desc : This loads the JPG file and returns it's data in a tImage struct
//=============================================================================
inline tImage *LoadJPGTexture(const char *strFileName)
{
    struct jpeg_decompress_struct cinfo;
    tImage *pImageData = NULL;
    FILE *pFile;
        
    // Open a file pointer to the jpeg file and check if it was found and opened 
    if((pFile = fopen(strFileName, "rb")) == NULL) 
    {
        // Display an error message saying the file was not found, then return NULL
        MessageBox(g_hWnd, "Unable to load JPG File!", "Error", MB_OK);
        return NULL;
    }
    
    // Create an error handler
    jpeg_error_mgr jerr;

    // Have our compression info object point to the error handler address
    cinfo.err = jpeg_std_error(&jerr);
    
    // Initialize the decompression object
    jpeg_create_decompress(&cinfo);
    
    // Specify the data source (Our file pointer)   
    jpeg_stdio_src(&cinfo, pFile);
    
    // Allocate the structure that will hold our eventual jpeg data (must free it!)
    pImageData = (tImage*)malloc(sizeof(tImage));

    // Decode the jpeg file and fill in the image data structure to pass back
    DecodeJPG(&cinfo, pImageData);
    
    // This releases all the stored memory for reading and decoding the jpeg
    jpeg_destroy_decompress(&cinfo);
    
    // Close the file pointer that opened the file
    fclose(pFile);

    // Return the jpeg data (remember, you must free this data after you are done)
    return pImageData;
}

//=============================================================================
//  Name :
//  Desc :
//=============================================================================
inline tImage *LoadTGATexture(const char *strFileName)
{
    return 0;
}
*/
