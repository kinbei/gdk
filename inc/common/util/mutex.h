#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <public.h>

/**
 * 
 */
class CMutex
{
public:
	/**
	 * 
	 */
	CMutex();
	/**
	 * 
	 */
	virtual ~CMutex();
	/**
	 * 
	 */
	void lock();
	/**
	 * 
	 */
	void unlock();

private:
	//
#ifdef WIN32
	CRITICAL_SECTION m_cs;
#else

#endif
};

#ifdef WINDOWS
	#include "mutex_win.hpp"
#else
	#include "mutex_unix.hpp"
#endif

/**
 * 
 */
class CAutoLock
{
public:
	/**
	 * 
	 */
	CAutoLock( CMutex* pLock ) : m_pLock( pLock )
	{
		if ( m_pLock != NULL )
			m_pLock->lock();
	}
	/**
	 * 
	 */
	virtual ~CAutoLock()
	{
		if ( m_pLock != NULL )
			m_pLock->unlock();
	}

private:
	//
	CMutex* m_pLock;
};

#endif
