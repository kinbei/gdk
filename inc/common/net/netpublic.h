#ifndef _NET_PUBLIC_H_
#define _NET_PUBLIC_H_

#include <public.h>
#include <basictype.h>

#define DISABLE_UNREFERENCE(x) (x)

#ifdef WINDOWS

	// 尽量保证 netpublic.h 在其它文件之前被 include, 避免在 win 下出现编译错误
	// This problem arises because windows.h (at least, that version of it) includes not winsock2.h but winsock.h; 
	// sadly when Microsoft wrote winsock2.h they chose neither to change windows.h to include winsock2.h, which replaces winsock.h, 
	// nor to include windows.h from winsock2.h and then add the definitions for the new Winsock 2 API methods & structures 
	// (this might seem reasonable since Winsock 2 does, strictly speaking, replace Winsock 1, 
	// but since the API must be fully backwards-compatible the distinction is somewhat meaningless and there's no real benefit to making winsock2.h standalone).
	// The fix is thankfully simple: always "#include <winsock2.h>" before windows.h.
	#include <winsock2.h>
	#include <windows.h>	

	typedef int socklen_t;

	#define GetLastNetError() WSAGetLastError()
	#define CloseSocket(x) ::closesocket(x)
	#define SHUT_RDWR SD_BOTH
	
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Mswsock.lib")

#else
	#include <sys/epoll.h>
	#include <fcntl.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h> // for ::close()

#ifndef SOCKET
	typedef int SOCKET;
#endif

	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
	#define GetLastNetError() errno
	#define CloseSocket(x) ::close(x)
	#define MAX_EPOLL_EVENTS 32
#endif

// Socket阻塞模式枚举
enum ESOCKET_MODE
{
	ASYNC_MODE,  // 异步模式
	SYNC_MODE,	 // 阻塞模式
};

#endif
