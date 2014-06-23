#ifndef _CONNECTOR_LISTENER_H_
#define _CONNECTOR_LISTENER_H_

/**
 * Connector 事件处理
 */
class IConnectorListener
{
public:
	/**
	 *
	 */
	IConnectorListener()
	{

	}

	/**
	 * 
	 */
	virtual ~IConnectorListener() 
	{

	}

	/**
	 * Connector 连接远程服务器成功后的处理
	 *
	 *  onOpen() 处理代码示例: 
	 * 	INetDriverPtr pNetDriver = NULL;
	 *	uint32 nErrCode = pNetDriver->registry( pConnector, this );
	 *	if ( nErrCode != 0 )
	 *	{
	 *		// 重连服务器 或 加入重连列表
	 *		pConnector->connect();
	 *		return ;
	 *	}
	 *
	 * \param pConnector
	 * \return 无
	 */
	virtual void onOpen( CConnectionPtr pConnection ) = 0;
};
typedef TRefCountToObj<IConnectorListener> IConnectorListenerPtr;

#endif
