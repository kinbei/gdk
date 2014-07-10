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
		FILE* file = NULL;
		if (   (  file = fopen("d:\\client.log", "ab" )  ) == NULL   )
			return ;

		fprintf( file, "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		fclose( file );
	}
	/**
	 * 
	 */
	virtual void info( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		FILE* file = NULL;
		if (   (  file = fopen("d:\\client.log", "ab" )  ) == NULL   )
			return ;

		fprintf( file, "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		fclose( file );
	}
	/**
	 * 
	 */
	virtual void error( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		FILE* file = NULL;
		if (   (  file = fopen("d:\\client.log", "ab" )  ) == NULL   )
			return ;

		fprintf( file, "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		fclose( file );
	}
	/**
	 * 
	 */
	virtual void warning( uint32 nThreadID, const char *pszFileName, uint32 nFileLine, const char* pszDebugInfo )
	{
		FILE* file = NULL;
		if (   (  file = fopen("d:\\client.log", "ab" )  ) == NULL   )
			return ;

		fprintf( file, "> thread(0x%08X) %s\r\n", nThreadID, pszDebugInfo );
		fclose( file );
	}
};

#endif
