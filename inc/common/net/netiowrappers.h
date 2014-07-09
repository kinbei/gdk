#ifndef _NET_IO_WRAPPERS_H_
#define _NET_IO_WRAPPERS_H_

#include <map>
#include <util/refcount.h>
#include <net/acceptor.h>
#include <net/connector.h>
#include <net/connection.h>

/**  
 * 网络IO模型抽象
 */

class INetIoWrappers
{
public:
	/**
	 *
	 */
	INetIoWrappers()
	{

	}
	
	/**
	 *
	 */
	virtual ~INetIoWrappers()
	{

	}

	/**
	 * 初始化
	 *
	 * \return 返回0表示成功, 否则表示失败
	 */
	virtual int32 init() = 0;

	/**
	 * 反初始化, 程序结束时调用
	 */
	virtual void uninit() = 0;

	/**
	 * 注册 Connector 事件的处理
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addConnector( CConnectorPtr pConnector ) = 0;

	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor ) = 0;

	/**
	 * 网络事件处理, 程序可以一直调用 run() 直至程序结束
	 *
	 * \param nTimeOutMilliseconds  超时的时间, 单位为毫秒
	 * \return 无
	 */
	virtual int32 run( int32 nTimeOutMilliseconds = 100 ) = 0;

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop() = 0;

	/**
	 * 
	 */
	virtual void getAllConnection( std::map<CConnection*, CConnectionPtr>& mapConnection ) = 0;
};
typedef TRefCountToObj<INetIoWrappers> INetIoWrappersPtr;

#endif
