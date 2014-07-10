#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#include <stdio.h>

#if defined(_WIN32)||defined(_WIN64)
	#define WINDOWS 
#else
	#define UNIX
#endif

#ifdef WINDOWS
	
#else
	#include <errno.h>
#endif

//////////////////////////////////////////////////////////////////////////
#ifdef WINDOWS
	#ifdef snprintf
		#undef snprintf
	#endif
	#define snprintf _snprintf_s
#else
	// do nothing
#endif

//////////////////////////////////////////////////////////////////////////
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#endif
