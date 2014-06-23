#ifndef _SOCKET_LIB_LOADER_H_
#define _SOCKET_LIB_LOADER_H_

#include <public.h>
#include <net/netpublic.h>

/**  
 * Socketø‚º”‘ÿ∆˜
 */

class CSocketLibLoader
{
public:
	/**
 	 * 
	 *
	 * \param 
	 * \return 
	 */
	CSocketLibLoader()
	{

	}

	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	virtual ~CSocketLibLoader()
	{
		unload();
	}

	/**
	 * ≥ı ºªØSocketø‚
	 *
	 * \param 
	 * \return 
	 */
	uint32 load()
	{
#ifdef WINDOWS

		WORD nVersionRequested = MAKEWORD( 2, 2 );
		WSADATA wsaData;

		// If successful, the WSAStartup function returns zero. Otherwise, it returns one of the error codes listed below. 
		// WSASYSNOTREADY The underlying network subsystem is not ready for network communication.
		// WSAVERNOTSUPPORTED The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
		// WSAEINPROGRESS A blocking Windows Sockets 1.1 operation is in progress.
		// WSAEPROCLIM A limit on the number of tasks supported by the Windows Sockets implementation has been reached.
		// WSAEFAULT The lpWSAData parameter is not a valid pointer.

		// The WSAStartup function directly returns the extended error code in the return value for this function. 
		// A call to the WSAGetLastError function is not needed and should not be used. 
		int nRet = ::WSAStartup( nVersionRequested, &wsaData );

		if ( nRet != 0 )
			return nRet;

		// ≈–∂œ∞Ê±æ∫≈
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 )
		{
			DISABLE_UNREFERENCE( ::WSACleanup() );
			return 1;
		}
#endif // WINDOWS

		return 0;
	}

	/**
	 *  Õ∑≈Socketø‚
	 *
	 * \param 
	 * \return 
	 */
	void unload()
	{
#ifdef WINDOWS
		DISABLE_UNREFERENCE( ::WSACleanup() );
#endif // WINDOWS
	}

private:

};

#endif
