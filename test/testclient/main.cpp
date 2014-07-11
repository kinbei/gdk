#include <net/netpublic.h>
#include <io/bytesbuffer.h>
#include <net/socketlibloader.h>
#include <net/netiowrappersfactory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include "myconnector.h"
#include "debuginfo.h"

#define GDK_VERSION_MAJOR 0
#define GDK_VERSION_MINOR 0
#define GDK_VERSION_REVISION 1
#define GDK_STRING_EXPAND(X) #X
#define GDK_STRING(X) GDK_STRING_EXPAND(X)
#define GDK_VERSION ( "GDK." GDK_STRING(GDK_VERSION_MAJOR) "." GDK_STRING(GDK_VERSION_MINOR) "." GDK_STRING(GDK_VERSION_REVISION) )

INetIoWrappersPtr g_pNetWrappers = CNetIoWrappersFactory::createInstance();

void sig_int(int sigi)
{
	g_pNetWrappers->stop();
}

// server ip
std::string g_strIP;
// server port
uint16 g_nPort;
// the number of send bytes to server per connection
long g_nConfPreConnectionSendBytes = 0;
// number of client connectiones
long g_nClientConnectionNumber = 1;

/**
 * 
 */
inline bool isNumber( const char *lpsz )
{
	while( '\0' != *lpsz )
	{
		if( ( *lpsz < '0' ) || ( *lpsz >'9' ) )
			return false;

		lpsz++;
	}

	return true;
}


/**
 * 获取命令行参数
 */
int32 parseCmdParam( int argc, char* argv[] )
{
	if ( argc != 5 )
		return -1;

	//
	g_strIP = argv[1];

	//
	if ( !isNumber( argv[2] ) )
		return -1;
	g_nPort = atoi( argv[2] );

	//
	if ( !isNumber( argv[3] ) )
		return -1;
	g_nConfPreConnectionSendBytes = atoi( argv[3] );

	//
	if ( !isNumber( argv[4] ) )
		return -1;
	g_nClientConnectionNumber = atoi( argv[4] );

	return 0;	
}

int main( int argc, char* argv[] )
{
	//
	CMyDebugPtr pDebug = new CMyDebug();
	if( pDebug->init() != 0 )
	{
		log_error("Failed to init debug module");
		exit(EXIT_FAILURE);
	}

	CDebugMgr::setDebug( pDebug.get() );
	

	log_info("VERSION: %s", GDK_VERSION);

	// 获取参数
	if ( parseCmdParam( argc, argv ) != 0 )
	{
		log_error("usage: %s IP Port CheckSendBytes ", argv[0]);
		exit(EXIT_FAILURE);
	}

	if( ( signal( SIGINT, sig_int ) == SIG_ERR ) || ( signal( SIGTERM, sig_int ) == SIG_ERR ) )
		exit(EXIT_FAILURE);

	int32 retCode = 0;

	if ( g_pNetWrappers == NULL )
	{
		log_error("Net Driver is null!");
		exit(EXIT_FAILURE);
	}

	// init network
	retCode = g_pNetWrappers->init();
	if ( retCode != 0 )
	{
		log_error("Failed to init NetDriver (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	CMyConnectorPtr* pConnector = new CMyConnectorPtr[ g_nClientConnectionNumber ];

	for ( int i = 0; i < g_nClientConnectionNumber; i++ )
	{
		// 
		pConnector[ i ] = new CMyConnector();
		
		retCode = pConnector[i]->open( g_strIP, g_nPort );
		if ( retCode != 0 )
		{
			log_error("Failed to init Acceptor (%d)", retCode);
			exit(EXIT_FAILURE);
		}

		// 
		retCode = g_pNetWrappers->addConnector( pConnector[i] );
		if ( retCode != 0 )
		{
			log_error("Failed to add Acceptor (%d)", retCode);
			exit(EXIT_FAILURE);
		}
	}

	log_info("Start Runing");

	//
	retCode = g_pNetWrappers->run();
	if ( retCode != 0 )
	{
		log_error("Failed to run NetDriver (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	g_pNetWrappers->uninit();

	// 
	delete[] pConnector;

	return EXIT_SUCCESS;
}
