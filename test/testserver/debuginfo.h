#ifndef _DEBUG_INFO_H_
#define _DEBUG_INFO_H_

#include <util/debug.h>

class CMyDebug : public IDebug
{
public:
	/**
	 * 
	 */
	CMyDebug()
	{

	}
	/**
	 * 
	 */
	virtual ~CMyDebug()
	{

	}
	/**
	 * 
	 */
	virtual void debug( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
 		fprintf( stdout, "> thread(%d) %s\n", nThreadID, pszDebugInfo);
 		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void info( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> thread(%d) %s\n", nThreadID, pszDebugInfo);
		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void error( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> thread(%d) %s\n", nThreadID, pszDebugInfo);
		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void warning( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> thread(%d) %s\n", nThreadID, pszDebugInfo);
		fflush(stdout);
	}
};

#endif
