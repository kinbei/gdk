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
	 * \param pConnector
	 * \return void
	 */
	virtual void onOpen( CConnectionPtr pConnection ) = 0;
};
typedef TRefCountToObj<IConnectorListener> IConnectorListenerPtr;

/**
 * 
 */
template< typename T >
class TConnectorListener : public IConnectorListener
{
public:
	/**
	 *
	 */
	TConnectorListener( T* pClass, 
		void (T::*pfnonOpen)( CConnectionPtr pConnection ) ) : 
	m_pClass( pClass ),
	m_pfnonOpen( pfnonOpen )
	{

	}

	/**
	 *
	 */
	virtual ~TConnectorListener()
	{

	}

	/**
	 * onAccept
	 */
	virtual void onOpen( CConnectionPtr pConnection )
	{
		(m_pClass->*m_pfnonOpen)( pConnection );
	}

private:
	//
	TConnectorListener() {}

	//
	T* m_pClass;
	//
	void (T::*m_pfnonOpen)( CConnectionPtr pConnection );
};


#endif
