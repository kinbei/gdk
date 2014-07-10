#ifndef _NET_IO_WRAPPERS_H_
#define _NET_IO_WRAPPERS_H_

#include <map>
#include <util/refcount.h>
#include <net/acceptor.h>
#include <net/connector.h>
#include <net/connection.h>

/**  
 * ����IOģ�ͳ���
 */

class INetIoWrappers
{
public:
	/**
	 *
	 */
	INetIoWrappers()
	{

	}
	
	/**
	 *
	 */
	virtual ~INetIoWrappers()
	{

	}

	/**
	 * ��ʼ��
	 *
	 * \return ����0��ʾ�ɹ�, �����ʾʧ��
	 */
	virtual int32 init() = 0;

	/**
	 * ����ʼ��, �������ʱ����
	 */
	virtual void uninit() = 0;

	/**
	 * ע�� Connector �¼��Ĵ���
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addConnector( CConnectorPtr pConnector ) = 0;

	/**
	 * 
	 *
	 * \param 
	 * \return 
	 */
	virtual int32 addAcceptor( CAcceptorPtr pAcceptor ) = 0;

	/**
	 * �����¼�����, �������һֱ���� run() ֱ���������
	 *
	 * \param nTimeOutMilliseconds  ��ʱ��ʱ��, ��λΪ����
	 * \return ��
	 */
	virtual int32 run( int32 nTimeOutMilliseconds = 100 ) = 0;

	/**
	 * stop
	 *
	 * \param 
	 * \return 
	 */
	virtual void stop() = 0;

	/**
	 * 
	 */
	virtual void getAllConnection( std::map<CConnection*, CConnectionPtr>& mapConnection ) = 0;
};
typedef TRefCountToObj<INetIoWrappers> INetIoWrappersPtr;

#endif
