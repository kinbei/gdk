#include <net/netpublic.h>
#include <io/bytesbuffer.h>
#include <net/socketlibloader.h>
#include <net/netiowrappersfactory.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include "myacceptor.h"
#include "debuginfo.h"

#define GDK_VERSION_MAJOR 0
#define GDK_VERSION_MINOR 0
#define GDK_VERSION_REVISION 1
#define GDK_STRING_EXPAND(X) #X
#define GDK_STRING(X) GDK_STRING_EXPAND(X)
#define GDK_VERSION ( "GDK." GDK_STRING(GDK_VERSION_MAJOR) "." GDK_STRING(GDK_VERSION_MINOR) "." GDK_STRING(GDK_VERSION_REVISION) )

#define ERROR_REPORT(fmt, ...) { fprintf( stderr, "> " fmt "\n", ##__VA_ARGS__); fflush(stderr); }
#define NORMAL_REPORT(fmt, ...) { fprintf( stdout, "> " fmt "\n", ##__VA_ARGS__); fflush(stdout); }

INetIoWrappersPtr g_pNetWrappers = CNetIoWrappersFactory::createInstance();

void sig_int(int sigi)
{
	g_pNetWrappers->stop();
}

// 绑定的IP
std::string g_strIP;
// 绑定的端口
uint16 g_nPort;
// 检查客户端发送的字节数
long g_nConfPreConnectionSendBytes = 0;

/**
 * 判断一个字符串是否为数字(不判断符号)
 *
 * \param 
 * \return 
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
	if ( argc != 4 )
		return -1;

	// 绑定IP
	g_strIP = argv[1];

	// 绑定端口
	if( !isNumber( argv[2] ) )
		return -1;
	g_nPort = atoi( argv[2] );

	// 检查客户端的字节数
	if( !isNumber( argv[3] ) )
		return -1;
	g_nConfPreConnectionSendBytes = atoi( argv[3] );

	return 0;	
}

int main( int argc, char* argv[] )
{
	NORMAL_REPORT("VERSION: %s", GDK_VERSION);

	// 获取参数
	if ( parseCmdParam( argc, argv ) != 0 )
	{
		ERROR_REPORT("usage: %s IP Port CheckSendBytes ", argv[0]);
		exit(EXIT_FAILURE);
	}

	int32 retCode = 0;
	CSocketLibLoader SockLibLoader;

	if( ( signal( SIGINT, sig_int ) == SIG_ERR ) || ( signal( SIGTERM, sig_int ) == SIG_ERR ) )
		exit(EXIT_FAILURE);

	// Socket库初始化
	retCode = SockLibLoader.load();
	if ( retCode != 0 )
	{
		ERROR_REPORT("Failed to loadSocketLib (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	if ( g_pNetWrappers == NULL )
	{
		ERROR_REPORT("Net Driver is null!");
		exit(EXIT_FAILURE);
	}

	// 网络初始化
	retCode = g_pNetWrappers->init();
	if ( retCode != 0 )
	{
		ERROR_REPORT("Failed to init NetDriver (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	// 创建 Accpetor 
	CMyAcceptorPtr pAcceptor = new CMyAcceptor();
	retCode = pAcceptor->open( g_strIP, g_nPort );
	if ( retCode != 0 )
	{
		ERROR_REPORT("Failed to init Acceptor (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	// 将 Acceptor 添加到网络通信模型
	retCode = g_pNetWrappers->addAcceptor( pAcceptor );
	if ( retCode != 0 )
	{
		ERROR_REPORT("Failed to add Acceptor (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	NORMAL_REPORT("Start Runing");

	//
	retCode = g_pNetWrappers->run( 100 );
	if ( retCode != 0 )
	{
		ERROR_REPORT("Failed to run NetDriver (%d)", retCode);
		exit(EXIT_FAILURE);
	}

	g_pNetWrappers->uninit();
	return EXIT_SUCCESS;
}
