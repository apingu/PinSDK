#ifndef __TZMESSAGE_H
#define __TZMESSAGE_H
//-----------------------------------------------------------------------------

// Include dependencies
#ifndef __RWCORE_H

struct GFV3d
{
    float x;   /**< X value */
    float y;   /**< Y value */
    float z;   /**< Z value */
};

#define __RWCORE_H
#endif

#ifndef __RTQUAT_H
//#include "rtquat.h"
#define __RTQUAT_H
#endif






///////////////////////////////////////////////////////////////////////////////
//  Properties
///////////////////////////////////////////////////////////////////////////////
#pragma pack (push, 1)
//-----------------------------------------------------------------------------
class TZPropCreatureDisplay
{
public:
    TZPropCreatureDisplay(const char* name, int race, int gender)
        : fRace(race), fGender(gender) { strcpy(fName, name); }

    ~TZPropCreatureDisplay() {}
public:
     int fRace;
     int fGender;
    char fName[32];
};
#pragma pack (pop)
//-----------------------------------------------------------------------------
class TZPropCreatureMove
{
public:
    TZPropCreatureMove(const GFV3d& pos)//, const RtQuat& rot)//, const RwV3d& vel
        : fPos(pos) {}//, fVel(vel), fOrient(rot) {}
public:
    GFV3d  fPos;
//    RtQuat fOrient;
//    RwV3d  fVel;
};

//-----------------------------------------------------------------------------
class TZPropCreatureAction
{
public:
    TZPropCreatureAction(int actionState)
        : fActionState(actionState) {}
public:
    int fActionState;
};

//-----------------------------------------------------------------------------
class TZPropCreature
{
public:
    TZPropCreature(const TZPropCreatureDisplay& display,
                   const TZPropCreatureMove&    move,
                   const TZPropCreatureAction&  action)
        : fDisplay(display), fMoveState(move), fActionState(action) {}

public:
    TZPropCreatureDisplay fDisplay;
    TZPropCreatureMove    fMoveState;
    TZPropCreatureAction  fActionState;
};

///////////////////////////////////////////////////////////////////////////////
//  Messages
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
class TZMessage
{
public:
    enum MsgType{ kInvalid, 
                  kCreatureDisplay, kCreatureMoveState, kCreatureActionState, kCreature,
                  kEntityDepart,
                  kHurt };

public:
    TZMessage(MsgType msgType, int id): fType(msgType), fIDSource(id) {} 
public:
    MsgType fType;
    int     fIDSource;
};

///////////////////////////////////////////////////////////////////////////////
// Messages of Creature
//-----------------------------------------------------------------------------
class TZMessageCreatureDisplay: public TZMessage
{
public:
    TZMessageCreatureDisplay(int idSource, const TZPropCreatureDisplay& propData)
        : TZMessage(kCreatureDisplay, idSource), fPropData(propData) {}

public:
    TZPropCreatureDisplay fPropData;
};

//-----------------------------------------------------------------------------
class TZMessageCreatureMove: public TZMessage
{
public:
    TZMessageCreatureMove(int idSource, const TZPropCreatureMove& propData, float timeStamp)
        : TZMessage(kCreatureMoveState, idSource), fPropData(propData), fTimeStamp(timeStamp) {}

public:
    float              fTimeStamp;
    TZPropCreatureMove fPropData;
};

//-----------------------------------------------------------------------------
class TZMessageCreatureAction: public TZMessage
{
public:
    TZMessageCreatureAction(int idSource, const TZPropCreatureAction& propData, float timeStamp)
        : TZMessage(kCreatureActionState, idSource), fPropData(propData), fTimeStamp(timeStamp) {}

public:
    float                fTimeStamp;
    TZPropCreatureAction fPropData;
};

//-----------------------------------------------------------------------------
class TZMessageCreature: public TZMessage
{
public:
    TZMessageCreature(int idSource, 
                      const TZPropCreatureDisplay& display,
                      const TZPropCreatureMove&    move,
                      const TZPropCreatureAction&  action,
                      float timeStamp)
        : TZMessage(kCreature, idSource), 
          fDisplay(display), fMoveState(move), fActionState(action), fTimeStamp(timeStamp) {}

public:
    float                 fTimeStamp;
    TZPropCreatureDisplay fDisplay;
    TZPropCreatureMove    fMoveState;
    TZPropCreatureAction  fActionState;
};

//-----------------------------------------------------------------------------
class TZMessageHurt: public TZMessage // TODO: Alex, Temp used
{
public:
    TZMessageHurt(int idSource, int state)
        : TZMessage(kHurt, idSource), fTarget(idSource), fHurtState(state) {}
        
public:
    int fTarget;
    int fHurtState;
};
 
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#endif