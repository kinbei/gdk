#include <net/iocp/wsaextensionfunction.h>

#ifdef WINDOWS

//
LPFN_ACCEPTEX CWSAExtensionFunction::m_lpfnAcceptEx = NULL;
//
LPFN_CONNECTEX CWSAExtensionFunction::m_lpfnConnectEx = NULL;
//
LPFN_GETACCEPTEXSOCKADDRS CWSAExtensionFunction::m_lpfnGetAcceptExSockaddrs = NULL;

#endif // WINDOWS
