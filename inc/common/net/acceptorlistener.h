#ifndef _ACCEPTOR_LISTENER_H_
#define _ACCEPTOR_LISTENER_H_

#include <util/refcount.h>
#include <net/connection.h>

/**
 * Acceptor�¼�����ӿ�
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
	 * �����µ����Ӵ���
	 *
	 * \param pConnection ��������������, ����ͨ�� pConnection->close() �ܾ�����, pConnection->setListener() ���ûص�����
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
