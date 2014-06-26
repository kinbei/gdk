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
	virtual void debug(  const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
 		fprintf( stdout, "> %s\n", pszDebugInfo);
 		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void info( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> %s\n", pszDebugInfo);
		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void error( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> %s\n", pszDebugInfo);
		fflush(stdout);
	}
	/**
	 * 
	 */
	virtual void warning( const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		fprintf( stdout, "> %s\n", pszDebugInfo);
		fflush(stdout);
	}
};

#endif
