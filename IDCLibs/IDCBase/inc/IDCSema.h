

#ifndef __IDC_SEMA_H__
#define __IDC_SEMA_H__

#include <semaphore.h>
#include "Types.h"

class CIdcSema
{
    
public:
    CIdcSema( uint32 initValue = 0 );
   
    virtual ~CIdcSema( void );

    bool WaitFor( time_t seconds, int32 nanoseconds = 0 );
    bool WaitUntil( time_t seconds, int32 nanoseconds );
    bool TryWait( void );
    void Post( void );

    int32 GetValue( void );

    void Reset( void );

private:
    sem_t handle;
};


#endif /*IDC_SEMA_H*/


