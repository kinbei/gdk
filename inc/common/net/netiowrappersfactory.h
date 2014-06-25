#ifndef _NET_IO_WRAPPERS_FACTORY_H_
#define _NET_IO_WRAPPERS_FACTORY_H_

#include <net/netiowrappers.h>
#include <net/iocp/iocpnetiowrappers.h>
#include <net/epoll/epollnetiowrappers.h>

/**  
 * ����IOģ�͹�����
 */

class CNetIoWrappersFactory
{
public:
	/**
	 * ����˵��
	 *
	 * \param 
	 * \return 
	 */
	static INetIoWrappersPtr createInstance()
	{
#ifdef WINDOWS
		return new CIocpNetIoWrappers();
#else
		return new CEpollNetIoWrappers();
#endif // WINDOWS
		
	}
};

#endif
