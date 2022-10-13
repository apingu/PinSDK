//Particle.cpp
//
//    patricle system
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

#include "Particle.h"
#include <math.h>

//PARTICLE particle[P_MAX]; // Our particles


/*
bool Init()
{
    // Try and load the first m_texture
    if(InitTexture(m_texture[0],"P1.bmp") == false)
        return false;

    // Try and load the second m_texture
    if(InitTexture(m_texture[1],"P2.bmp") == false)
        return false;

    // Set second textures color to a "yellowish color"
    m_texture[1].red = 200 + rand()%25;
    m_texture[1].green = 200 + rand()%25;
    m_texture[1].blue = 0;

    GLfloat angle = 0; // A particle's angle
    GLfloat speed = 0; // A particle's speed

    // Create all the particles
    for(int i = 0; i < P_MAX; i++)
    {
        speed = float(rand()%50 + 50); // Make a random speed

        // Init the particle with a random speed
        InitParticle(particle[i],speed,angle);

        angle += 360 / (float)P_MAX; // Increment the angle so when all the particles are
                                    // initialized they will be equally positioned in a
                                   // circular fashion
    }

    return true; // Particles and textures have been initialized
}
*/

//=============================================================================
//  Name : 
//  Desc : Initializes a particle
//=============================================================================
void VfParticle::InitParticle( PARTICLE& particle, Pfloat speed, Pfloat angle )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    particle.speed = speed; // Set the particle's speed
    particle.angle = angle; // Set the particle's current angle of rotation

    // Randomly set the particles color
    particle.red = rand() % 255;
    particle.green = rand() % 255;
    particle.blue = rand() % 255;
#endif
}


//=============================================================================
//  Name : 
//  Desc : Load the specified m_texture and set m_texture's color to SOLID white
//=============================================================================
bool VfParticle::InitTexture( TEXTURE& m_texture, char* fileName )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    // If the file name is not valid, can't initialize m_texture
    if( !fileName )
        return false;

    AUX_RGBImageRec* image  = NULL; // This will hold the "image" we're loading

    image = auxDIBImageLoad( fileName ); // Load the image

    // Error Check
    if( !image )
        return false;

    glGenTextures( 1, &( m_texture.tID ) ); // Create a m_texture
    glBindTexture( GL_TEXTURE_2D, m_texture.tID ); // The m_texture is going to be a 
    // 2D m_texture so say so

    // Specify the 2D m_texture map
    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  3,
                  image->sizeX,
                  image->sizeY,
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  image->data );

    // Make our textures look purty when we apply them to "objects" that are bigger OR
    // smaller than the m_texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // Free Memory
    if( image )
    {
        if( image->data )
            free( image->data );

        free( image );
    }

    // Set the m_texture to color to SOLID white
    m_texture.red = m_texture.green = m_texture.blue = 255;

#endif
    return true;
}


//=============================================================================
//  Name : 
//  Desc : Draw a particle
//=============================================================================
void VfParticle::DrawParticle( const PARTICLE& particle )
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    // First draw the FIRST part of our particle

    glBindTexture( GL_TEXTURE_2D, m_texture[ 0 ].tID ); // Bind 1st m_texture
    glColor3ub( particle.red, particle.green, particle.blue ); // Use particle's color

    // Calculate the current x any y positions of the particle based on the particle's
    // current angle -- This will make the particles move in a "circular pattern"
    GLfloat xPos= sinf( particle.angle ); 
    GLfloat yPos= cosf( particle.angle );

    // Translate to the x and y position and the #defined PDEPTH (particle depth)
    glTranslatef( xPos, yPos, PDEPTH );

    // Draw the first quad
    glBegin( GL_QUADS );

    glTexCoord2f( 0, 0 );
    glVertex3f( -1, 1, 0 );

    glTexCoord2f( 1, 0 );
    glVertex3f( 1, 1, 0 );

    glTexCoord2f( 1, 1 );
    glVertex3f( 1, -1, 0 );

    glTexCoord2f( 0, 1 );
    glVertex3f( -1, -1, 0 );

    glEnd(); // Done drawing quad

    // Draw the SECOND part of our particle

    glBindTexture( GL_TEXTURE_2D, m_texture[ 1 ].tID ); // Bind 2nd m_texture
    glColor3ub( m_texture[ 1 ].red, m_texture[ 1 ].green, m_texture[ 1 ].blue ); // Use 2nd m_texture's color

    glRotatef( particle.angle, 0, 0, 1 ); // Rotate around the z-axis (depth axis)

    // Draw the second quad
    glBegin( GL_QUADS );

    glTexCoord2f( 0, 0 );
    glVertex3f( -1, 1, 0 );

    glTexCoord2f( 1, 0 );
    glVertex3f( 1, 1, 0 );

    glTexCoord2f( 1, 1 );
    glVertex3f( 1, -1, 0 );

    glTexCoord2f( 0, 1 );
    glVertex3f( -1, -1, 0 );

    glEnd(); // Done drawing quad

    // Translate back to where we began
    glTranslatef( -xPos, -yPos, -PDEPTH );

#endif
}


//=============================================================================
//  Name : 
//  Desc : Simply draws/updates all the particles (ie effectively runs the application)
//=============================================================================
void VfParticle::RunApp()
{
    for( Puint i = 0;i < m_particle.size();i++ )
    {
        DrawParticle( m_particle[ i ] );

        // Increment the particle's angle
        m_particle[ i ].angle += ANGLE_INC;
    }
}


//=============================================================================
//  Name : 
//  Desc : Free up the memory allocated by the textures
//=============================================================================
void VfParticle::FreeTextures()
{
#if defined (_WGLKERNEL_) || defined (_XGLKERNEL_)
    if( m_texture[ 0 ].tID )
        glDeleteTextures( 1, &( m_texture[ 0 ].tID ) );

    if( m_texture[ 1 ].tID )
        glDeleteTextures( 1, &( m_texture[ 1 ].tID ) );
#endif
}


/*  Extra technical stuff...

    When you run the app you'll notice that the particles spin around either getting closer
    or farther away from the center of the screen.  This is simply a byproduct of using 
    glRotatef().  Every time you call glRotatef() the "current matrix" (in our case the model
    view matrix) will get multiplied by a rotation matrix, this resulting matrix becomes the
    new model view matrix so each call to glRotatef() is changing this matrix, thus changing
    where the particles appear on the screen.

*/
