#include <io/bytesbuffer.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <typeinfo>

#ifndef WINDOWS
	#include <unistd.h>
#endif

CBytesBuffer::CBytesBuffer()
{
	// 缓冲区指针
	m_pBuf = NULL;
	// 缓冲区长度
	m_nBufSize = 0;

	// 存放数据的开始位置
	m_pDataBeginPos = NULL;
	// 存放数据的大小
	m_nDataSize = 0;

	// 写的长度
	m_nWriteSize = 0;
}

CBytesBuffer::~CBytesBuffer()
{
	if ( m_pBuf != NULL )
		free( m_pBuf );
}

char* CBytesBuffer::getRowDataPointer()
{
	// 还未分配内存
	if ( m_pBuf == NULL || m_pDataBeginPos == NULL )
		return NULL;

	return m_pDataBeginPos;
}

void CBytesBuffer::popBytes( int32 nSize )
{
	m_pDataBeginPos += nSize;
	m_nDataSize -= nSize;
}

char* CBytesBuffer::writebegin( int32 nWriteSize )
{
	if ( m_pBuf == NULL )
	{
		// 未分配内存
		int32 nAllocSize = getAllocSize( nWriteSize );

		m_pBuf = (char*)malloc( nAllocSize );
		//
		m_nBufSize = nAllocSize;

		//
		m_pDataBeginPos = m_pBuf;
		// 
		m_nDataSize = 0;

		// 
		m_nWriteSize = nWriteSize;

		char *pWriteBeginPos = m_pDataBeginPos + m_nDataSize;
		char *pWriteEndPos = pWriteBeginPos + nWriteSize;
		char *pBufBegin = m_pBuf;
		char *pBufEnd = m_pBuf + m_nBufSize;

		return pWriteBeginPos;
	}
	else
	{
		// 合法检测 m_pDataBeginPos 必须在 m_pBuf 与 m_pBuf + m_nBufSize 之间
		char *pWriteBeginPos = m_pDataBeginPos + m_nDataSize;
		char *pWriteEndPos = pWriteBeginPos + nWriteSize;
		char *pBufBegin = m_pBuf;
		char *pBufEnd = m_pBuf + m_nBufSize;

		if ( pWriteEndPos > pBufEnd )
		{
			int32 nFreeSize = m_nBufSize  - m_nDataSize;

			if ( nFreeSize >= nWriteSize )
			{
				// 有足够的空间, 移动内存
				memmove( m_pBuf, m_pDataBeginPos, m_nDataSize );

				m_pDataBeginPos = m_pBuf;

				// 
				m_nWriteSize = nWriteSize;
			}
			else
			{
				// 空间不足, 申请更大内存
				int32 nAllocSize = getAllocSize( m_nBufSize + nWriteSize );

				int32 nDataBeginPosOffSet = m_pDataBeginPos - m_pBuf;

				char* pNewBuf = (char*)realloc( m_pBuf, nAllocSize );
				if ( pNewBuf == NULL )
					return NULL;

				m_pBuf = pNewBuf;
				//
				m_nBufSize = nAllocSize;

				//
				m_pDataBeginPos = m_pBuf + nDataBeginPosOffSet;
				// 
				m_nDataSize;

				// 
				m_nWriteSize = nWriteSize;

			}

			pWriteBeginPos = m_pDataBeginPos + m_nDataSize;
		}
		else
		{
			m_pBuf;
			//
			m_nBufSize;

			//
			m_pDataBeginPos;
			// 
			m_nDataSize;

			// 
			m_nWriteSize = nWriteSize;
		}

		return pWriteBeginPos;
	}
}

void CBytesBuffer::writecommit( void )
{
	m_nDataSize += m_nWriteSize;
	m_nWriteSize = 0;
}

int32 CBytesBuffer::getDataSize() const
{
	return m_nDataSize;
}

void CBytesBuffer::clear()
{
	// 存放数据的开始位置
	m_pDataBeginPos = m_pBuf;
	// 存放数据的大小
	m_nDataSize = 0;
}

int32 CBytesBuffer::getAllocSize( int32 nSize /*= 4096 */ )
{
	if ( nSize < 4096 )
		nSize = 4096;

	long nPageSize = 0;

#ifdef WINDOWS
	SYSTEM_INFO si;
	::GetSystemInfo( &si );
	nPageSize = si.dwPageSize;
#else
	nPageSize = sysconf( _SC_PAGESIZE );
#endif
	
	// 获取最接近内存页大小倍数的值
	return ( (  (nSize + nPageSize - 1) / nPageSize ) * nPageSize );
}

bool CBytesBuffer::compact()
{
	int32 nAllocSize = getAllocSize( m_nDataSize );

	if ( m_nBufSize > nAllocSize )
	{
		int32 nDataBeginPosOffSet = m_pDataBeginPos - m_pBuf;

		char* pNewBuf = (char*)realloc( m_pBuf, nAllocSize );
		if ( pNewBuf == NULL )
			return false;

		m_pBuf = pNewBuf;
		//
		m_nBufSize = nAllocSize;

		//
		m_pDataBeginPos = m_pBuf + nDataBeginPosOffSet;
		// 
		m_nDataSize;

		// 
		m_nWriteSize = 0;
	}

	return true;
}
