#ifndef _CONNECTOR_LISTENER_H_
#define _CONNECTOR_LISTENER_H_

/**
 * Connector �¼�����
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
	 * Connector ����Զ�̷������ɹ���Ĵ���
	 *
	 *  onOpen() �������ʾ��: 
	 * 	INetDriverPtr pNetDriver = NULL;
	 *	uint32 nErrCode = pNetDriver->registry( pConnector, this );
	 *	if ( nErrCode != 0 )
	 *	{
	 *		// ���������� �� ���������б�
	 *		pConnector->connect();
	 *		return ;
	 *	}
	 *
	 * \param pConnector
	 * \return ��
	 */
	virtual void onOpen( CConnectionPtr pConnection ) = 0;
};
typedef TRefCountToObj<IConnectorListener> IConnectorListenerPtr;

#endif
