#ifndef _CONNECTION_LISTENER_H_
#define _CONNECTION_LISTENER_H_

class CConnection;
typedef TRefCountToObj<CConnection> CConnectionPtr; 

/**
 * Connection�¼�����ӿ�
 */
class IConnectionListener
{
public:
	/**
	 *
	 */
	IConnectionListener()
	{

	}

	/**
	 *
	 */
	virtual ~IConnectionListener()
	{

	}

	/**
	 * onRecv
	 *
	 * \param pConnection
	 * you can call send()/setListener()/getAddress()/getHandle()/close() of pConnection
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection, CBytesBufferPtr pRecvBuffer );

	/**
	 * onSendCompleted
	 *
	 * \param 
	 * \return 
	 */
	virtual void onSendCompleted( CConnectionPtr pConnection, uint32 nSendBytes ) = 0;

	/**
	 * onClose
	 *
	 * \param 
	 * \return 
	 */
	virtual void onClose( CConnectionPtr pConnection ) = 0;
};
typedef TRefCountToObj<IConnectionListener> IConnectionListenerPtr; 

/**
 * 
 */
template< typename T >
class TConnectionListener : public IConnectionListener
{
public:
	/**
	 *
	 */
	TConnectionListener( T* pClass, 
		void (T::*pfnonSendCompleted)( CConnectionPtr pConnection, uint32 nSendBytes ),
		void (T::*pfnonRecvCompleted)( CConnectionPtr pConnection ),
		void (T::*pfnonClose)( CConnectionPtr pConnection ) ):
	m_pClass( pClass ),
	m_pfnonSendCompleted( pfnonSendCompleted ),
	m_pfnonRecvCompleted( pfnonRecvCompleted ),
	m_pfnonClose( pfnonClose )
	{

	}

	/**
	 *
	 */
	virtual ~TConnectionListener()
	{

	}
	
	/**
	 * onSend
	 *
	 * \param 
	 * \return 
	 */
	virtual void onSendCompleted( CConnectionPtr pConnection, uint32 nSendBytes )
	{
		(m_pClass->*m_pfnonSendCompleted)( pConnection, nSendBytes );
	}

	/**
	 * onRecv
	 *
	 * \param 
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection )
	{
		(m_pClass->*m_pfnonRecvCompleted)( pConnection );
	}

	/**
	 * onClose
	 *
	 * \param 
	 * \return 
	 */
	virtual void onClose( CConnectionPtr pConnection )
	{
		(m_pClass->*m_pfnonClose)( pConnection );
	}

private:
	//
	TConnectionListener() {}

	//
	T* m_pClass;
	//
	void (T::*m_pfnonSendCompleted)( CConnectionPtr pConnection, uint32 nSendBytes );
	//
	void (T::*m_pfnonRecvCompleted)( CConnectionPtr pConnection );
	//
	void (T::*m_pfnonClose)( CConnectionPtr pConnection );
};

#endif
