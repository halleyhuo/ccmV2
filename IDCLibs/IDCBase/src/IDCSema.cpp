
#include "Types.h"

#include "IDCSema.h"
#include <errno.h>
#include <stdio.h>

#include <time.h>

const int64 nanoSec2SecRadio = 1000000000;

CIdcSema::CIdcSema( uint32 initValue /*=0*/ )
{
    if ( sem_init( &handle, 0, initValue ) == -1 )
    {
        perror("Init Semphore:");
    }    
}


CIdcSema::~CIdcSema( void )
{   
    if ( sem_destroy( &handle ) == -1 )
    {
        perror("destroy Semphore:");
    }    
}

int32 CIdcSema::GetValue( void )
{    
    int32 result = 0;
    if ( sem_getvalue( const_cast<sem_t*>( &handle ), &result ) == -1 )
    {
        perror("Get Semphore:");
    }   

    return result;
}

bool CIdcSema::WaitFor( time_t seconds, int32 nanoseconds )
{ 
    timespec waitTime;
	for(int32 i=0; i<3; i++)
	{
		if ( clock_gettime( CLOCK_REALTIME, &waitTime ) == -1 )
		{
			if( (errno == EINTR) || (errno==EAGAIN) )
			{
				continue;//retry
			}
			else
			{
				perror("error on clock gettime:");
				return false;
			}
		}
		else
		{
			break;
		}
	}

    waitTime.tv_sec += seconds;
    waitTime.tv_nsec += nanoseconds;

    return WaitUntil( waitTime.tv_sec, waitTime.tv_nsec );
}


bool CIdcSema::WaitUntil( time_t seconds, int32 nanoseconds )
{        
    uint32 tmpNanoSec = nanoseconds;
    int32 tmpSecs    = seconds;
    
    //usually overflow when self plus, includes nanosec<0 and >1000000000
    if(tmpNanoSec>(uint32)nanoSec2SecRadio)
    {
        int32 exchangeSec = tmpNanoSec/nanoSec2SecRadio;
        tmpSecs += exchangeSec;
        int32 offNanoSec  = tmpNanoSec%nanoSec2SecRadio;
        tmpNanoSec = offNanoSec;
    }
    timespec waitTime;
    waitTime.tv_sec = tmpSecs;
    waitTime.tv_nsec = tmpNanoSec;
    
    for(int32 i=0; i<3; i++)
    {
        if ( sem_timedwait( &handle, &waitTime ) == -1 )
        {
            if ( errno == ETIMEDOUT )
            {                
                return false;
            }
            else if( (errno == EINTR) || (errno==EAGAIN) )
            {                
                continue;
            }
            else
            {
                return false;
            }            
        }
        else
        {
            return true;
        }
    }   
    return false;
}

bool CIdcSema::TryWait( void )
{  
    if ( sem_trywait( &handle ) == -1 )
    {
        if ( errno == EAGAIN )
        {            
            return false;
        }        
    }    
    return true;
}

void CIdcSema::Post( void )
{
	for(int32 i=0; i<3; i++)
	{
		if ( sem_post( &handle ) == -1 )
		{
			//Interrupt by Signal, retry
			if( (errno == EINTR) || (errno==EAGAIN) )
			{
				continue;//retry
			}
			else
			{
				perror("post CIdcSema");
				break;
			}
		}    
		else
		{
			break;
		}
	}
}

void CIdcSema::Reset( void )
{
    while ( TryWait() )
    {
        // fetch all semaphores until TryWait fails
    }   
}

