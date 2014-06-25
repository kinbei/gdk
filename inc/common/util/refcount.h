#ifndef _REF_COUNT_H_
#define _REF_COUNT_H_

#include <assert.h> // for assert()
#include <public.h>

#ifdef WINDOWS
	#include <Windows.h>
#endif

#include <typeinfo>

#ifdef REF_COUNT_TRACK_LOG_SUPPORT
	#define REF_COUNT_TRACK(fmt, ...) TRACK_LOG(fmt, ##__VA_ARGS__)
#else
	#define REF_COUNT_TRACK(fmt, ...)  
#endif

// verify that types are complete for increased safety
template<class T> inline void checked_delete(T * x)
{
	// intentionally complex - simplification causes regressions
	typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
	(void) sizeof(type_must_be_complete);
	delete x;
}

/**
 * 
 */
class CRefCountBase
{
private:

	// No copy constructor
	CRefCountBase( CRefCountBase const & );
	CRefCountBase & operator= ( CRefCountBase const & );

	//
	long m_nRefCount;

public:
	/**
	 * 
	 */
	CRefCountBase(): m_nRefCount( 1 )
	{
	}

	/**
	 * 
	 */
	virtual ~CRefCountBase()
	{
	}

	/**
	 * 
	 */
	virtual void dispose() = 0;

	/**
	 * 
	 */
	void incRef()
	{
#ifdef WINDOWS
		( ::InterlockedIncrement( &m_nRefCount ) );
#else
		__sync_fetch_and_add( &m_nRefCount, 1 );
#endif		

		REF_COUNT_TRACK("CRefCount::incRef() m_nCount(%d)", m_nCount);
	}

	/**
	 * 
	 */
	void decRef() // nothrow
	{
#ifdef WINDOWS
		long nRefCount = ::InterlockedDecrement( &m_nRefCount );
#else
		long nRefCount = __sync_fetch_and_sub( &m_nRefCount, 1 );
#endif

		REF_COUNT_TRACK("CRefCount::decRef() m_nCount(%d)", nRefCount);

		if ( nRefCount == 0 )
		{
			dispose();
			delete this;
		}
	}

	/**
	 * 
	 */
	long getRefCount() const
	{
		return static_cast<long const volatile &>( m_nRefCount );
	}
};

/**
 * 
 */
template<class T> 
class TRefCountImpl: public CRefCountBase
{
private:

	T * m_pPointer;

	// No copy constructor
	TRefCountImpl( TRefCountImpl const & );
	TRefCountImpl & operator= ( TRefCountImpl const & );

public:
	/**
	 * 
	 */
	explicit TRefCountImpl( T * px ): m_pPointer( px )
	{
	}

	/**
	 * 
	 */
	virtual void dispose()
	{
		checked_delete( m_pPointer );
	}
};

/**
 * 
 */
class CRefCount
{
private:
	//
	CRefCountBase* m_pRefCountBase;

public:
	/**
	 * 
	 */
	CRefCount(): m_pRefCountBase( NULL ) 
	{
	}

	/**
	 * 
	 */
	template<class Y> explicit CRefCount( Y * p ): m_pRefCountBase( NULL )
	{
		m_pRefCountBase = new TRefCountImpl<Y>( p );
	}

	/**
	 * 
	 */
	CRefCount(CRefCount const & r): m_pRefCountBase(r.m_pRefCountBase)
	{
		if( m_pRefCountBase != NULL ) 
			m_pRefCountBase->incRef();
	}

	/**
	 * 
	 */
	~CRefCount()
	{
		if( m_pRefCountBase != NULL ) 
			m_pRefCountBase->decRef();
	}

	/**
	 * 
	 */
	CRefCount & operator= (CRefCount const & r)
	{
		CRefCountBase * tmp = r.m_pRefCountBase;

		if( tmp != m_pRefCountBase )
		{
			if( tmp != NULL ) 
				tmp->incRef();
			
			if( m_pRefCountBase != NULL ) 
				m_pRefCountBase->decRef();

			m_pRefCountBase = tmp;
		}

		return *this;
	}

	/**
	 * 
	 */
	friend inline bool operator==(CRefCount const & a, CRefCount const & b)
	{
		return a.m_pRefCountBase == b.m_pRefCountBase;
	}

	/**
	 * 
	 */
	friend inline bool operator<(CRefCount const & a, CRefCount const & b) 
	{
		return a.m_pRefCountBase < b.m_pRefCountBase;
	}

	/**
	 * 
	 */
	void swap(CRefCount & r)
	{
		CRefCountBase * tmp = r.m_pRefCountBase;
		r.m_pRefCountBase = m_pRefCountBase;
		m_pRefCountBase = tmp;
	}

	/**
	 * 
	 */
	long getRefCount() const
	{
		return m_pRefCountBase != NULL ? m_pRefCountBase->getRefCount(): 0;
	}

	/**
	 * 
	 */
	void incRef()
	{
		if ( m_pRefCountBase != NULL )
			m_pRefCountBase->incRef();
	}

	/**
	 * 
	 */
	void decRef()
	{
		if ( m_pRefCountBase != NULL )
			m_pRefCountBase->decRef();
	}
};

/**
 * 
 */
template < class T >
class TRefCountToObj 
{
public:
	/**
	 * 
	 */
	TRefCountToObj() : m_pRefObj( NULL ), m_RefCounter()
	{
		REF_COUNT_TRACK("%s TRefCountToObj()", typeid(T).name());
	}

	/**
	 * typedef TRefCountToObj<CXXX> CXXXPtr;
	 * CXXXPtr pTest1 ( new CXXX() );
	 */
	//!NOTE why share_ptr add explicit keyword before this function?
	template<class Y>
	TRefCountToObj( Y * p ): m_pRefObj( p ), m_RefCounter() 
	{
		CRefCount( p ).swap( m_RefCounter );
	}

	/**
	 * 	(void) dynamic_cast< T* >( static_cast< U* >( 0 ) );
	 *	T* p = dynamic_cast< T* >( r.get() );
	 *	TRefObject<T>( r, p )
	 */
    template< class Y >
    TRefCountToObj( TRefCountToObj<Y> const & r, T *p )  : m_pRefObj( p ), m_RefCounter( r.m_RefCounter )
    {
    }

	//!NOTE Why not share_ptr not support this interface, not find any problem yet
	template< class Y >
	TRefCountToObj( TRefCountToObj<Y> const & r )  : m_pRefObj( r.m_pRefObj ), m_RefCounter( r.m_RefCounter )
	{
	}

	/**
	 * 
	 */
	TRefCountToObj& operator =( const TRefCountToObj& rhs )
	{
		REF_COUNT_TRACK("%s operator =(const TRefCountToObj& Right)", typeid(T).name());

		TRefCountToObj<T>(rhs).swap(*this);
		return *this;
	}

	/**
	 * 
	 */
    template<class Y>
    TRefCountToObj & operator=(TRefCountToObj<Y> const & rhs) 
    {
        TRefCountToObj<T>(rhs).swap(*this);
        return *this;
    }

	/**
	 * typedef TRefCountToObj<CXXX> CXXXPtr;
	 * CXXXPtr pTest = NULL;
	 *
	 * \param 
	 * \return 
	 */
	TRefCountToObj& operator = ( T* rhs )
	{
		m_pRefObj = rhs;
		CRefCount( rhs ).swap( m_RefCounter );

		return *this;
	}
	
	/**
	 * 
	 */
	bool operator! () const 
	{
		return m_pRefObj == NULL;
	}

	/**
	 * 
	 */
	friend inline bool operator<(TRefCountToObj<T> const & a, TRefCountToObj<T> const & b) 
	{
		return a.m_RefCounter < b.m_RefCounter;
	}

	/**
	 * 
	 */
    template<class Y> 
	void reset( Y * p ) 
    {
        assert( p == NULL || p != m_pRefObj );
        TRefCountToObj<T>( p ).swap( *this );
    }

	/**
	 * 
	 */
    template<class Y> void reset( TRefCountToObj<Y> const & r, T *p )
    {
        TRefCountToObj<T>( r, p ).swap( *this );
    }

	/**
	 * 
	 */
    T& operator* () const
    {
        assert( m_pRefObj != NULL );
        return *m_pRefObj;
    }
    
	/**
	 * 
	 */
    T* operator-> () const 
    {
		assert( m_pRefObj != NULL );
        return m_pRefObj;
    }

	/**
	 * 
	 */
    T* get() const 
    {
        return m_pRefObj;
    }

	// implicit conversion to "bool"
// 	typedef T* TRefCountToObj<T>::*unspecified_bool_type;
// 	operator unspecified_bool_type() const 
// 	{
// 		return m_pRefObj == NULL ? NULL: &TRefCountToObj<T>::m_pRefObj;
// 	}

	operator T*() const 
	{
		return m_pRefObj;
	}


	/**
	 * 
	 */
    long getRefCount() const 
    {
        return m_RefCounter.getRefCount();
    }

	/**
	 * 
	 */
    void swap( TRefCountToObj & other ) 
    {
		if ( &m_pRefObj != &other.m_pRefObj )
		{	
			T* tmp = m_pRefObj;
			m_pRefObj = other.m_pRefObj;
			other.m_pRefObj = tmp;
		}

        m_RefCounter.swap(other.m_RefCounter);
    }
	
	/**
	 *
	 */
	virtual ~TRefCountToObj()
	{
		REF_COUNT_TRACK( "%s ~TRefCountToObj()", typeid(T).name())
	}

	/**
	 * 
	 */
	void incRef()
	{
		m_RefCounter.incRef();
	}

	/**
	 * 
	 */
	void decRef()
	{
		m_RefCounter.decRef();
	}

protected:
	template<class Y> friend class TRefCountToObj;

	T* m_pRefObj;			   // contained pointer
	CRefCount m_RefCounter;    // reference counter

};

template<class T, class U> TRefCountToObj<T> dynamic_pointer_cast( TRefCountToObj<U> const & r ) 
{
	(void) dynamic_cast< T* >( static_cast< U* >( 0 ) );
	T* p = dynamic_cast< T* >( r.get() );
	return p ? TRefCountToObj<T>( r, p ) : TRefCountToObj<T>();
}

#endif
