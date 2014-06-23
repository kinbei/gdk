#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <net/netpublic.h>
#include <net/socket.h>
#include <util/refcount.h>
#include <string>

/**
 * Tcp Socket 实现
 */
class CTCPSocket : public ISocket
{
public:
	/**
	 *
	 */
	CTCPSocket();

	/**
	 * // explicit 关键字防止参数被隐式转换
	 */
	explicit CTCPSocket( SOCKET s );

	/**
	 * 
	 */
	virtual ~CTCPSocket();

	/**
	 * 创建socket, 成功返回0, 失败返回非0 
	 */
	virtual int32 create();

	/**
	 * 绑定端口
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 bind( const struct sockaddr *SockAddr, int nLen );

	/**
	 * 关闭socket
	 */
	virtual void close();

	/**
	 * 连接服务器
	 *
	 * \return 0表示成功, 非0表示错误码
	 */
	virtual int32 connect( const char *lpstrIP, uint16 nPort );

	/**
	 * 监听端口
	 */
	virtual int32 listen( const std::string& strIP, uint16 nPort, int nBackLog = SOMAXCONN );

	/**
	 * 接受连接
	 *
	 * \return 如果失败, 返回NULL, 非NULL时需要 delete
	 */
	virtual CTCPSocket* accept( struct sockaddr *lpAddr, socklen_t *nAddrlen );

	/**
	 * 发送数据操作
	 *
	 */
	virtual int32 send( const char* lpBuf, uint32 nBufLen );

	/**
	 * 接收数据操作
	 */
	virtual uint32 recv( const char* lpBuf, uint32 nBufLen );
};
typedef TRefCountToObj<CTCPSocket> CTCPSocketPtr;

#endif
