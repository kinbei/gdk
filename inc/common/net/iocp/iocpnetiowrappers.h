#ifndef _IOCP_NET_IO_WRAPPERS_H_
#define _IOCP_NET_IO_WRAPPERS_H_

#include <vector>
#include <net/netpublic.h>
#include <net/netiowrappers.h>
#include <net/acceptor.h>
#include <net/iocp/wsaextensionfunction.h>

#ifdef WINDOWS

#include <net/iocp/iocpdef.h>
#include <util/debug.h>

/**
* IOCP封装
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
	* 初始化
	*
	* \return 返回0表示成功, 否则表示失败
	*/
	virtual int32 init();

	/**
	* 反初始化, 程序结束时调用
	*/
	virtual void uninit();

	/**
	* 注册 Connector 事件的处理
	*
	* \param 
	* \return 
	*/
	virtual int32 addConnector( CConnectorPtr pConnector );

	/**
	* 函数说明
	*
	* \param 
	* \return 
	*/
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor );

	/**
	* 网络事件处理, 程序可以一直调用此函数直至程序结束
	*
	* \param nTimeOutMilliseconds  超时的时间, 单位为毫秒
	* \return 无
	*/
	virtual int32 run( int32 nTimeOutMilliseconds = 100 );

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop();

	/**
	 * 
	 */
	virtual void getAllConnection( std::map<CConnection*, CConnectionPtr>& mapConnection );

protected:
	/**
	 * 获取系统CPU个数
	 *
	 * \param 
	 * \return 
	 */
	uint32 getCPUCount();


	/**
	 * 工作线程
	 *
	 * \param 
	 * \return 
	 */
	static UINT WINAPI WorkerThread( LPVOID pParam );

	/**
	 * 投递Send
	 *
	 * \param 
	 * \return 
	 */
	int32 postSend( CConnectionPtr pConnection );

	/**
	* 投递 Connect 请求
	*
	* \param 
	* \return 
	*/
	int32 postConnect( CConnectorPtr pConnector );

	/**
	* 投递Recv请求
	*
	* \param 
	* \return 
	*/
	int32 postRecv( CConnectionPtr pConnection );

	/**
	* 投递Accept请求
	*
	* \param 
	* \return 
	*/
	int32 postAccept( CAcceptorPtr pAcceptor );

	/**
	 * 
	 */
	void onConnectionClose( CConnectionPtr pConnection );

private:
	// 
	HANDLE m_hIOCompletionPort;
	// for resource release
	std::vector<CAcceptorPtr> m_vecAcceptor;
	// for resource release
	std::vector<CConnectorPtr> m_vecConnector;
	// the map of connection, for resource release and get all of connection
	std::map< CConnection*, CConnectionPtr > m_mapConnection;
};
typedef TRefCountToObj<CIocpNetIoWrappers> CIocpNetIoWrappersPtr;

#endif // WINDOWS

#endif
