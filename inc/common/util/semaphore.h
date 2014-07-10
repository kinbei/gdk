#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

class CSemaphore
{
public:
	/**
	 * 
	 */
	CSemaphore()
	{
		//
		m_hSem = INVALID_HANDLE_VALUE;
	}
	/**
	 * 
	 */
	virtual ~CSemaphore()
	{

	}
	/**
	 * 
	 */
	int32 init()
	{
		m_hSem = CreateSemaphore( NULL, 0, 1, NULL );
		
		// If the function succeeds, the return value is a handle to the semaphore object
		// If the function fails, the return value is NULL. 
		if ( m_hSem == NULL )
			return GetLastError();

		return 0;
	}
	/**
	 * 
	 */
	void wait()
	{
		(void)( WaitForSingleObject( m_hSem, INFINITE ) );
	}
	/**
	 * 
	 */
	void release()
	{
		ReleaseSemaphore( m_hSem, 1, NULL );
	}

private:
	//
	HANDLE m_hSem;
};

#endif
