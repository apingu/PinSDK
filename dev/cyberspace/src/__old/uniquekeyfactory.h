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
//
//
#ifndef UNIQUEKEYFACTORY_H
#define UNIQUEKEYFACTORY_H

typedef long ukey_t;

ukey_t CreateUKey( void );
bool   CancelUKey( ukey_t key );
bool   VerifyUKey( ukey_t key );
bool   InsertUKey( ukey_t key );

#endif
