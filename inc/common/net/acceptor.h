#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <basictype.h>
#include <net/tcpsocket.h>
#include <net/acceptorlistener.h>

/**
 * ���ӽ�����
 * ������������
 */
class CAcceptor
{
public:
	/**
	 * 
	 */
	CAcceptor();

	/**
	 * 
	 */
	virtual ~CAcceptor();

	/**
	 * ��ʼ�����˿�
	 *
	 * \param lpstrIP ������IP��ַ
	 * \param nPort �����Ķ˿�
	 * \param nBackLog 
	 * \return ����0��ʾ�ɹ�, ��0��ʾʧ��
	 */
	int32 open( const char *lpstrIP, uint16 nPort, int nBackLog );

	/**
	 * ��ȡ���
	 *
	 * \param 
	 * \return 
	 */
	virtual SOCKET getHandle();

	/**
	 * �ر�
	 */
	void close();

	/**
	 * ���ü����¼�
	 *
	 * \param 
	 * \return 
	 */
	void setListener( IAcceptorListenerPtr pListener );

	/**
	 * ��ȡ�����¼�
	 *
	 * \param 
	 * \return 
	 */
	IAcceptorListenerPtr getListener();

private:
	// ����Socket
	CTCPSocketPtr m_pListenSocket;
	// �����¼�
	IAcceptorListenerPtr m_pListener;
};
typedef TRefCountToObj<CAcceptor> CAcceptorPtr;

#endif
