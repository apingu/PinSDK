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

typedef unsigned long UniqueId_t;

UniqueId_t CreateUKey( void );
bool       CancelUKey( UniqueId_t key );
bool       VerifyUKey( UniqueId_t key );
bool       InsertUKey( UniqueId_t key );

#endif
