#ifndef _SCOPE_PTR_H_
#define _SCOPE_PTR_H_

template < class T >
class CScopePtr 
{
public:
	/**
	 * 
	 */
	CScopePtr( T* pObj )
	{
		//
		m_nCount = 0;
		//
		m_pObj = pObj;
		//
		incRef();
	}
	/**
	 * 
	 */
	virtual ~CScopePtr()
	{
		if ( m_pObj != NULL )
			decRef();
	}
	/**
	 * 增加引用计数
	 */
	virtual void incRef()
	{
		m_nCount++;
	}

	/**
	 * 减少引用计数
	 */
	virtual void decRef()
	{
		m_nCount--;

		if ( m_nCount == 0 )
			delete m_pObj;
	}
	/**
	 * 
	 */
	T* operator->()
	{
		return m_pObj;
	}

private:
	//
	T* m_pObj;
	//
	long m_nCount;
};

#endif
