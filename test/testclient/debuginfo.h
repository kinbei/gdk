#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include <util/debug.h>
#include <util/semaphore.h>
#include <util/mutex.h>

class CMyDebug : public IDebug
{
public:
	/**
	 * 
	 */
	CMyDebug()
	{
		m_sem.init();
	}
	/**
	 * 
	 */
	virtual ~CMyDebug()
	{

	}
	static UINT WINAPI WorkerThread( LPVOID pParam )
	{
		CMyDebug* pThis = static_cast<CMyDebug*>( pParam );

		while( true )
		{
			pThis->m_sem.wait();

			pThis->m_lock.lock();
			std::vector<std::string> vecLog;
			vecLog = pThis->m_vecLog;
			pThis->m_lock.unlock();

			FILE* file = NULL;
			if (   (  file = fopen("d:\\client.log", "ab" )  ) == NULL   )
				continue ;

			std::vector<std::string>::iterator iter_t;
			for ( iter_t = vecLog.begin(); iter_t != vecLog.end(); iter_t++ )
				fprintf( file, iter_t->c_str() );

			fclose( file );
		}

		return 0;
	}
	/**
	 * 
	 */
	int32 init()
	{
		CThreadPtr pThread = CThreadFactory::createThread();
		return pThread->initialize( CMyDebug::WorkerThread, this );
	}
	/**
	 * 
	 */
	virtual void debug( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		m_lock.lock();
		char szBuf[1024] = {0};
		snprintf( szBuf, sizeof(szBuf), "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		m_vecLog.push_back( szBuf );
		m_lock.unlock();

		m_sem.release();
	}
	/**
	 * 
	 */
	virtual void info( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		m_lock.lock();
		char szBuf[1024] = {0};
		snprintf( szBuf, sizeof(szBuf), "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		m_vecLog.push_back( szBuf );
		m_lock.unlock();

		m_sem.release();
	}
	/**
	 * 
	 */
	virtual void error( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		m_lock.lock();
		char szBuf[1024] = {0};
		snprintf( szBuf, sizeof(szBuf), "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		m_vecLog.push_back( szBuf );
		m_lock.unlock();

		m_sem.release();
	}
	/**
	 * 
	 */
	virtual void warning( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		m_lock.lock();
		char szBuf[1024] = {0};
		snprintf( szBuf, sizeof(szBuf), "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		m_vecLog.push_back( szBuf );
		m_lock.unlock();

		m_sem.release();
	}

private:
	//
	CSemaphore m_sem;
	//
	CMutex m_lock;
	//
	std::vector<std::string> m_vecLog;
};

typedef TRefCountToObj<CMyDebug> CMyDebugPtr;

#endif
