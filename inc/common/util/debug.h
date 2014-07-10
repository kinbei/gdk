#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <basictype.h>
#include <util/thread.h>
#include <util/refcount.h>
#include <stdarg.h>

#ifdef WINDOWS
	#define WRITE_VAR_LOG( buffer, fmt ) \
		va_list arglist; \
		va_start( arglist, fmt ); \
		int nRet = _vsnprintf_s( buffer, sizeof(buffer)-1, fmt, arglist ); \
		if ( nRet > 0 ) \
		{ \
			buffer[ nRet ] = 0; \
		} \
		else if ( nRet == -1 ) \
		{ \
			buffer[ sizeof(buffer) - 1 ] = 0; \
		} \
		va_end( arglist ); \

#else
	#define WRITE_VAR_LOG( buffer, fmt ) \
		va_list arglist; \
		va_start( arglist, fmt ); \
		int nRet = vsnprintf( buffer, sizeof(buffer), fmt, arglist ); \
		buffer[ sizeof(buffer) - 1 ] = 0 ; \
		va_end( arglist ); \

#endif

/**
 * 
 */
class IDebug
{
public:
	/**
	 * 
	 */
	IDebug()
	{

	}
	/**
	 * 
	 */
	virtual ~IDebug()
	{

	}
	/**
	 * 
	 */
	virtual void debug( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void info( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void error( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void warning( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
};
typedef TRefCountToObj<IDebug> IDebugPtr;

/**
 * 
 */
class CDebugMgr
{
public:
	/**
	 * 
	 */
	CDebugMgr()
	{

	}
	/**
	 * 
	 */
	virtual ~CDebugMgr()
	{

	}
	/**
	 * 
	 */
	static void setDebug( IDebugPtr pDebug )
	{
		ms_pDebug = pDebug;
	}
	/**
	 * 
	 */
	static void debug( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->debug( nThreadID, pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void info( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->info( nThreadID, pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void error( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->error( nThreadID, pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void warning( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->warning( nThreadID, pszFileName, nFileLine, szBuf );
		}
	}

private:
	//
	static IDebugPtr ms_pDebug;
};


#define log_debug( fmt, ... ) CDebugMgr::debug( getCurrentThreadID(), __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_info( fmt, ... ) CDebugMgr::info( getCurrentThreadID(), __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_error( fmt, ... ) CDebugMgr::error( getCurrentThreadID(), __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_warning( fmt, ... ) CDebugMgr::warning( getCurrentThreadID(), __FILE__, __LINE__, fmt, ##__VA_ARGS__ )

#endif
