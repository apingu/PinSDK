//Particle.cpp
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
//
//
//                                               Copyright    (C)    2000    Pin
//
#ifndef PARTICLE_H
#define PARTICLE_H

//#include <Kernel3D.h>
#include <vector>
#include <pdef.h>
#include <VfwGraphic.h>



#define PDEPTH -20 // The depth of the particles along the z-axis
//#define P_MAX 12 // Max number of particles

#define ANGLE_INC .15f // How much we increment a particle's angle by each frame

// This is the structure that will hold all the information about our
// particle
struct PARTICLE
{
    Pfloat  speed; // How fast the particle is traveling
    Pfloat  angle; // Current angle of rotation of the particle

    // Particle's color
    Pbyte   red;
    Pbyte   green;
    Pbyte   blue;
};

// This is our TEXTURE struct that will hold a texture and a default color to be applied
// the texture
struct TEXTURE
{
    Puint   tID; // The texture's ID

    // A default color to be applied on top of the texture
    Pbyte   red;
    Pbyte   green;
    Pbyte   blue;
};

bool Init(); // Inits the particles and textures -- If successful returns true, else
// returns false

class VfParticle
{
    std::vector<PARTICLE>   m_particle; // Our particles
    std::vector<TEXTURE>    m_texture; // We have two textures
    //Puint m_maxnum;


public:
    // Loads the texture specified by "fileName" and sets the default texture color to SOLID white
    bool                    InitTexture( TEXTURE& texture, Pstr fileName );
    void                    InitParticle( PARTICLE& particle,
                                          Pfloat speed,
                                          Pfloat angle ); // Inits a particle

    // Draws a particle
    void                    DrawParticle( const PARTICLE& particle );

    // Runs the application (ie makes the screen look purty :)
    void                    RunApp();

    void                    FreeTextures(); // Free the textures we allocated
};

#endif
