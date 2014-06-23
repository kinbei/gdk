#ifndef _BYTES_BUFFER_H_
#define _BYTES_BUFFER_H_

#include <basictype.h>
#include <common/util/refcount.h>
#include <assert.h>

/**
 * CBytesBuffer
 */

class CBytesBuffer
{
public:
	/**
	 * 
	 */
	CBytesBuffer();

	/**
	 * 
	 */
	virtual ~CBytesBuffer();

	/**
	 * �ӻ�������ȡָ���ֽ���
	 * !NOTE ע�ⷵ��ֵ����ΪNULL, ��ȡ��ָ�������Χ [ 0, getDataSize() ]
	 *
	 * \param nReadSize Ҫ��ȡ���ֽ���
	 * \return ���ض�ȡ������ָ��, ����NULL��ʾʧ��
	 */
	char* getRowDataPointer();

	/**
	 * ��ɶ�ȡ, ���ӻ�����ͷ����ɾ�Ѿ���ȡ���ڴ�
	 */
	virtual void popBytes( int32 nSize );

	/**
	 * ��ʼ�������Ķ�����
	 *
	 * \param nLen Ҫд�������ֽ���
	 * \return ���ز�����ָ��, ����NULL���ʾʧ��
	 */
	char* writebegin( int32 nWriteSize );

	/**
	 * ��ɶԻ�������д����
	 */
	void writecommit( void );

	/**
	 * ���ػ������Ч����
	 */
	virtual int32 getDataSize() const;

	/**
	 * ��ջ�����
	 */
	void clear();

	/**
	 * ѹ���ڴ�, �ͷŶ�����ڴ�
	 */
	bool compact();

protected:
	/**
	 * 
	 */
	int32 getAllocSize( int32 nSize = 4096 );

private:
	// ������ָ��
	char *m_pBuf;
	// ����������
	int32 m_nBufSize;

	// ������ݵĿ�ʼλ��
	char *m_pDataBeginPos;
	// ������ݵĴ�С
	int32 m_nDataSize;

	// д�ĳ���
	int32 m_nWriteSize;
};
typedef TRefCountToObj<CBytesBuffer> CBytesBufferPtr;

#endif
