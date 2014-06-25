#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <basictype.h>
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
	virtual void debug( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void info( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void error( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
	/**
	 * 
	 */
	virtual void warning( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo ) = 0;
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
	static void debug( const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->debug( pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void info( const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->info( pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void error( const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->error( pszFileName, nFileLine, szBuf );
		}
	}
	/**
	 * 
	 */
	static void warning( const char *pszFileName, uint32 nFileLine, const char *pszFmt, ... )
	{
		if ( ms_pDebug != NULL )
		{
			char szBuf[1024] = {0};
			WRITE_VAR_LOG( szBuf, pszFmt );
			ms_pDebug->warning( pszFileName, nFileLine, szBuf );
		}
	}

private:
	//
	static IDebugPtr ms_pDebug;
};


#define log_debug( fmt, ... ) CDebugMgr::debug( __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_info( fmt, ... ) CDebugMgr::info( __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_error( fmt, ... ) CDebugMgr::error( __FILE__, __LINE__, fmt, ##__VA_ARGS__ )
#define log_warning( fmt, ... ) CDebugMgr::warning( __FILE__, __LINE__, fmt, ##__VA_ARGS__ )

#endif
