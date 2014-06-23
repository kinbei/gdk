#ifndef _BASIC_TYPE_H_
#define _BASIC_TYPE_H_

// Basic Data Type
#if defined(_WIN32) || defined(_WIN64)

	typedef unsigned char uint8;
	typedef unsigned __int16 uint16;
	typedef unsigned __int32 uint32;
	typedef unsigned __int64 uint64;

	typedef signed char int8;
	typedef signed __int16 int16;
	typedef signed __int32 int32;
	typedef signed __int64 int64;

#else

	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned long uint32;
	typedef unsigned long long uint64;

	typedef signed char int8;
	typedef signed short int16;
	typedef signed long int32;
	typedef signed long long int64;

#endif

// maximum value and minimum value
#define UMAX_VAL(T)  ((T) -1)
#define MAX_VAL(T)   ((T) (((u ## T) -1) >> 1))
#define UMIN_VAL(T)  ((T) 0)
#define MIN_VAL(T)   ((T) ~MAX_VAL(T))

#define MAX_UINT8  UMAX_VAL(uint8)
#define MAX_UINT16 UMAX_VAL(uint16)
#define MAX_UINT32 UMAX_VAL(uint32)
#define MAX_UINT64 UMAX_VAL(uint64)

#define MIN_UINT8  UMIN_VAL(uint8)
#define MIN_UINT16 UMIN_VAL(uint16)
#define MIN_UINT32 UMIN_VAL(uint32)
#define MIN_UINT64 UMIN_VAL(uint64)

#define MAX_INT8  MAX_VAL(int8)
#define MAX_INT16 MAX_VAL(int16)
#define MAX_INT32 MAX_VAL(int32)
#define MAX_INT64 MAX_VAL(int64)

#define MIN_INT8  MIN_VAL(int8)
#define MIN_INT16 MIN_VAL(int16)
#define MIN_INT32 MIN_VAL(int32)
#define MIN_INT64 MIN_VAL(int64)

// 基本类型的高低位操作
#define GET_UINT64_LOW32(x)  ((uint32)((x)&0xFFFFFFFF))
#define GET_UINT64_HIGH32(x) ((uint32)(((x)&0xFFFFFFFF00000000)>>32))
#define GET_UINT32_LOW16(x)  ((uint16)((x)&0xFFFF))
#define GET_UINT32_HIGH16(x) ((uint16)(((x)&0xFFFF0000)>>16))
#define GET_UINT16_LOW8(x)   ((uint8)((x)&0xFF))
#define GET_UINT16_HIGH8(x)  ((uint8)(((x)&0xFF00)>>8))
#define GET_UINT8_LOW4(x)    ((uint8)((x)&0xF))
#define GET_UINT8_HIGH4(x)   ((uint8)(((x)&0xF0)>>4))

#define MAKE_UINT64(high, low) (   ((uint64)(GET_UINT64_LOW32(high)) << 32) | ((uint64)(GET_UINT64_LOW32(low)))   )
#define MAKE_UINT32(high, low) (   ((uint32)(GET_UINT32_LOW16(high)) << 16) | ((uint32)(GET_UINT32_LOW16(low)))   )
#define MAKE_UINT16(high, low) (   ((uint16)(GET_UINT16_LOW8(high))  << 8)  | ((uint16)(GET_UINT16_LOW8(low)))    )
#define MAKE_UINT8(high, low)  (   ((uint8) (GET_UINT8_LOW4(high))   << 4)  | ((uint8)(GET_UINT8_LOW4(low)))      )

#endif
