#ifndef _ACCEPTOR_LISTENER_H_
#define _ACCEPTOR_LISTENER_H_

#include <util/refcount.h>
#include <net/connection.h>

/**
 * Acceptor事件处理接口
 */
class IAcceptorListener
{
public:
	/**
	 *
	 */
	IAcceptorListener()
	{

	}

	/**
	 *
	 */
	virtual ~IAcceptorListener()
	{

	}

	/**
	 * 接受新的连接处理
	 *
	 * \param pConnection 连上来的新连接, 可以通过 pConnection->close() 拒绝连接, pConnection->setListener() 设置回调函数
	 * \return 
	 */
	virtual void onAccept( CConnectionPtr pConnection ) = 0 ;
};
typedef TRefCountToObj<IAcceptorListener> IAcceptorListenerPtr; 

/**
 * 
 */
template< typename T >
class TAcceptorListener : public IAcceptorListener
{
public:
	/**
	 *
	 */
	TAcceptorListener( T* pClass, 
		void (T::*pfnonAccept)( CConnectionPtr pConnection ) ) : 
	m_pClass( pClass ),
	m_pfnonAccept( pfnonAccept )
	{

	}

	/**
	 *
	 */
	virtual ~TAcceptorListener()
	{

	}

	/**
	 * onAccept
	 */
	virtual void onAccept( CConnectionPtr pConnection )
	{
		(m_pClass->*m_pfnonAccept)( pConnection );
	}

private:
	//
	TAcceptorListener() {}

	//
	T* m_pClass;
	//
	void (T::*m_pfnonAccept)( CConnectionPtr pConnection );
};


#endif
