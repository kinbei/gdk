#ifndef _ATOM_H_
#define _ATOM_H_

#include <public.h>

// increment
inline uint32 atomIncrement( uint32* var ) 
{
#ifdef WINDOWS
	return InterlockedIncrement( (long *)(var) );
#else
	return __sync_add_and_fetch( (long *)(var), 1);
#endif
}

// decrement
inline uint32 atomDecrement( uint32* var )
{
#ifdef WINDOWS
	return InterlockedDecrement( (long *)(var) );
#else
	return __sync_add_and_fetch( (long *)(var), -1);
#endif
}

// set value, windows returns the new value, Linux returns the old value
inline uint32 atomSet( long* var, const uint32 value) 
{
#ifdef WINDOWS
	::InterlockedExchange( (long *)(var), (long)(value));
#else
	__sync_lock_test_and_set( (long *)(var), value);
#endif
	return value;
}

// get value
inline uint32 atomGet( uint32* var) 
{
#ifdef WINDOWS
	return InterlockedExchangeAdd((long *)(var), 0);
#else
	return __sync_fetch_and_add((uint32 *)(var), 0);
#endif
}


#endif
