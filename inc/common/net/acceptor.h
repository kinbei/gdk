#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include <basictype.h>
#include <net/tcpsocket.h>
#include <net/acceptorlistener.h>

/**
 * 连接接收器
 * 被动建立连接
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
	 * 开始监听端口
	 *
	 * \param lpstrIP 监听的IP地址
	 * \param nPort 监听的端口
	 * \param nBackLog 
	 * \return 返回0表示成功, 非0表示失败
	 */
	int32 open( const char *lpstrIP, uint16 nPort, int nBackLog );

	/**
	 * 获取句柄
	 *
	 * \param 
	 * \return 
	 */
	virtual SOCKET getHandle();

	/**
	 * 关闭
	 */
	void close();

	/**
	 * 设置监听事件
	 *
	 * \param 
	 * \return 
	 */
	void setListener( IAcceptorListenerPtr pListener );

	/**
	 * 获取监听事件
	 *
	 * \param 
	 * \return 
	 */
	IAcceptorListenerPtr getListener();

private:
	// 监听Socket
	CTCPSocketPtr m_pListenSocket;
	// 监听事件
	IAcceptorListenerPtr m_pListener;
};
typedef TRefCountToObj<CAcceptor> CAcceptorPtr;

#endif
