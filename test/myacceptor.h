#ifndef _MY_ACCEPTOR_H_
#define _MY_ACCEPTOR_H_

#include <net/acceptor.h>
#include <util/debug.h>

#define RETURN_IF_FAILED( x ) \
	do \
	{ \
		int32 nRet = ( x ); \
		if ( nRet != 0 ) \
			return nRet; \
	} while ( false ); \

/**
 * 
 */
class CMyAcceptor : public CAcceptor
{
public:
	/**
	 * 
	 */
	CMyAcceptor()
	{
	}
	/**
	 * 
	 */
	virtual ~CMyAcceptor()
	{

	}
	
	/**
	 *
	 * \param 
	 * \return 
	 */
	int32 open( const std::string& strIP, uint16 nPort )
	{
		RETURN_IF_FAILED( CAcceptor::open( strIP, nPort ) );

		this->setListener( new TAcceptorListener<CMyAcceptor>( this, &CMyAcceptor::onAccept ) );
		return 0;
	}

protected:
	/**
	 * �����µ����Ӵ���
	 *
	 * \param pConnection ��������������, ����ͨ�� pConnection->close() �ܾ�����, pConnection->setListener() ���ûص�����
	 * \return 
	 */
	virtual void onAccept( CConnectionPtr pConnection )
	{
		// ָ�����ӵ����紦���¼�
		pConnection->setListener( new TConnectionListener<CMyAcceptor>( this, 
			&CMyAcceptor::onSendCompleted, 
			&CMyAcceptor::onRecvCompleted, 
			&CMyAcceptor::onClose ) ); 

		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress == NULL )
		{
			DEBUG_INFO( "pAddress is null" );
			return ;
		}

		DEBUG_INFO( "new Connection: %s", pAddress->asString() );
	}

	/**
	 * onRecv
	 *
	 * \param 
	 * \return 
	 */
	virtual void onRecvCompleted( CConnectionPtr pConnection )
	{
		CBytesBufferPtr pBuffer = pConnection->getRecvBuffer();
		if ( pBuffer == NULL )
		{
			DEBUG_INFO( "recv buffer is null" );
			return ;
		}

		DEBUG_INFO( pBuffer->getRowDataPointer() );
		pBuffer->clear();
	}

	/**
	 * onSend
	 *
	 * \param 
	 * \return 
	 */
	virtual void onSendCompleted( CConnectionPtr pConnection, uint32 nSendBytes )
	{

	}

	/**
	 * onClose
	 *
	 * \param 
	 * \return 
	 */
	virtual void onClose( CConnectionPtr pConnection )
	{
		CAddressPtr pAddress = pConnection->getAddress();
		if ( pAddress != NULL )
			DEBUG_INFO( "Connection(%s) closed", pAddress->asString() );
	}

private:

};
typedef TRefCountToObj<CMyAcceptor> CMyAcceptorPtr;

#endif
