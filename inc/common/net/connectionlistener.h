#ifndef _CONNECTION_LISTENER_H_
#define _CONNECTION_LISTENER_H_

class CConnection;
typedef TRefCountToObj<CConnection> CConnectionPtr; 

/**
 * Connection事件处理接口
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
	 * onRecvCompleted
	 *
	 * \param 
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection, const char *pBuf, uint32 nRecvLen ) = 0;

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
		void (T::*pfnonRecvCompleted)( CConnectionPtr pConnection, const char *pBuf, uint32 nRecvLen ),
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
	virtual void onRecvCompleted( CConnectionPtr pConnection, const char *pBuf, uint32 nLen )
	{
		(m_pClass->*m_pfnonRecvCompleted)( pConnection, pBuf, nLen );
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
	void (T::*m_pfnonRecvCompleted)( CConnectionPtr pConnection, const char *pBuf, uint32 nRecvLen );
	//
	void (T::*m_pfnonClose)( CConnectionPtr pConnection );
};

#endif
