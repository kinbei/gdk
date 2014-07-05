#ifdef WINDOWS

#ifndef _MUTEX_WIN_HPP_
#define _MUTEX_WIN_HPP_

inline CMutex::CMutex()
{
	InitializeCriticalSection( &m_cs );
}

inline CMutex::~CMutex()
{
	DeleteCriticalSection( &m_cs );
}

inline void CMutex::lock()
{
	EnterCriticalSection( &m_cs );
}

inline void CMutex::unlock()
{
	LeaveCriticalSection( &m_cs );
}

#endif // _MUTEX_WIN_HPP_

#endif // WINDOWS
