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
	 * 从缓冲区读取指定字节数
	 * !NOTE 注意返回值可能为NULL, 获取的指针操作范围 [ 0, getDataSize() ]
	 *
	 * \param nReadSize 要读取的字节数
	 * \return 返回读取操作的指针, 返回NULL表示失败
	 */
	char* getRowDataPointer();

	/**
	 * 完成读取, 并从缓冲区头部移删已经读取的内存
	 */
	virtual void popBytes( int32 nSize );

	/**
	 * 开始缓冲区的读操作
	 *
	 * \param nLen 要写入的最大字节数
	 * \return 返回操作的指针, 返回NULL则表示失败
	 */
	char* writebegin( int32 nWriteSize );

	/**
	 * 完成对缓冲区的写操作
	 */
	void writecommit( void );

	/**
	 * 返回缓存的有效长度
	 */
	virtual int32 getDataSize() const;

	/**
	 * 清空缓冲区
	 */
	void clear();

	/**
	 * 压缩内存, 释放多余的内存
	 */
	bool compact();

protected:
	/**
	 * 
	 */
	int32 getAllocSize( int32 nSize = 4096 );

private:
	// 缓冲区指针
	char *m_pBuf;
	// 缓冲区长度
	int32 m_nBufSize;

	// 存放数据的开始位置
	char *m_pDataBeginPos;
	// 存放数据的大小
	int32 m_nDataSize;

	// 写的长度
	int32 m_nWriteSize;
};
typedef TRefCountToObj<CBytesBuffer> CBytesBufferPtr;

#endif
