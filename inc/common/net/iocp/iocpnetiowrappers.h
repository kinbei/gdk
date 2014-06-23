#ifndef _IOCP_NET_IO_WRAPPERS_H_
#define _IOCP_NET_IO_WRAPPERS_H_

#include <vector>
#include <net/netpublic.h>
#include <net/netiowrappers.h>
#include <net/acceptor.h>
#include <net/iocp/wsaextensionfunction.h>

#ifdef WINDOWS

#include <process.h>
#include <net/iocp/iocpdef.h>
#include <util/debug.h>

/**
* IOCP��װ
*/
class CIocpNetIoWrappers : public INetIoWrappers
{
public:
	/**
	* 
	*
	* \param 
	* \return 
	*/
	CIocpNetIoWrappers();

	/**
	* 
	*
	* \param 
	* \return 
	*/
	virtual ~CIocpNetIoWrappers();

	/**
	* ��ʼ��
	*
	* \return ����0��ʾ�ɹ�, �����ʾʧ��
	*/
	virtual int32 init();

	/**
	* ����ʼ��, �������ʱ����
	*/
	virtual void uninit();

	/**
	* ע�� Connector �¼��Ĵ���
	*
	* \param 
	* \return 
	*/
	virtual int32 addConnector( CConnectorPtr pConnector );

	/**
	* ����˵��
	*
	* \param 
	* \return 
	*/
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor );

	/**
	* �����¼�����, �������һֱ���ô˺���ֱ���������
	*
	* \param nTimeOutMilliseconds  ��ʱ��ʱ��, ��λΪ����
	* \return ��
	*/
	virtual int32 run( int32 nTimeOutMilliseconds );

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop();

protected:
	/**
	 * ��ȡϵͳCPU����
	 *
	 * \param 
	 * \return 
	 */
	uint32 getCPUCount();


	/**
	 * �����߳�
	 *
	 * \param 
	 * \return 
	 */
	static UINT WINAPI WorkerThread( LPVOID pParam );

	/**
	 * Ͷ��Send
	 *
	 * \param 
	 * \return 
	 */
	int32 postSend( CConnectionPtr pConnection );

	/**
	* Ͷ�� Connect ����
	*
	* \param 
	* \return 
	*/
	int32 postConnect( CConnectorPtr pConnector );

	/**
	* Ͷ��Recv����
	*
	* \param 
	* \return 
	*/
	int32 postRecv( CConnectionPtr pConnection );

	/**
	* Ͷ��Accept����
	*
	* \param 
	* \return 
	*/
	int32 postAccept( CAcceptorPtr pAcceptor );

private:
	// ��ɶ˿�
	HANDLE m_hIOCompletionPort;
	// Acceptor�б�,  ������Դ�ͷ�
	std::vector<CAcceptorPtr> m_vecAcceptor;
	// Connector�б�, ������Դ�ͷ�
	std::vector<CConnectorPtr> m_vecConnector;
};
typedef TRefCountToObj<CIocpNetIoWrappers> CIocpNetIoWrappersPtr;

#endif // WINDOWS

#endif
