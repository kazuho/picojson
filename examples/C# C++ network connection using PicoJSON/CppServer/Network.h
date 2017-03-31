#pragma once

#define _WINSOCKAPI_
#include <WS2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <stdexcept>

#define AcceptParams ( __in SOCKET s, __out_bcount_opt(*addrlen) struct sockaddr FAR * addr, __inout_opt int FAR * addrlen )
#define BindParams ( __in SOCKET s, __in_bcount(namelen) const struct sockaddr FAR * name, __in int namelen )
#define CloseSocketParams ( __in SOCKET s )
#define HtonsParams ( __in u_short hostshort )
#define HtonlParams ( __in u_short hostlong )
#define ListenParams ( __in SOCKET s, __in int backlog )
#define RecvParams ( __in SOCKET s, __out_bcount_part(len, return) __out_data_source(NETWORK) char FAR * buf, __in int len, __in int flags )
#define SelectParams ( __in int nfds, __inout_opt fd_set FAR * readfds, __inout_opt fd_set FAR * writefds, __inout_opt fd_set FAR * exceptfds, __in_opt const struct timeval FAR * timeout )
#define SendParams ( __in SOCKET s, __in_bcount(len) const char FAR * buf, __in int len, __in int flags )
#define SocketParams ( __in int af, __in int type, __in int protocol )
#define WSACleanupParams ( void )
#define WSAGetLastErrorParams ( void )
#define WSAStartupParams ( __in WORD wVersionRequested, __out LPWSADATA lpWSAData )

class Net
{
private:
    static Net* instance;

#ifdef dynamic_linking
    HMODULE hLib;
    int (__stdcall *accept) AcceptParams;
    int (__stdcall *bind) BindParams;
    int (__stdcall *closesocket) CloseSocketParams;
    unsigned short (__stdcall *htons) HtonsParams;
    unsigned short (__stdcall *htonl) HtonlParams;
    int (__stdcall *listen) ListenParams;
    int (__stdcall *recv) RecvParams;
    int (__stdcall *select) SelectParams;
    int (__stdcall *send) SendParams;
    SOCKET (__stdcall *socket) SocketParams;
    int (__stdcall *WSACleanup) WSACleanupParams;
    int (__stdcall *WSAGetLastError) WSAGetLastErrorParams;
    int (__stdcall *WSAStartup) WSAStartupParams;
#endif


	Net() 
	{
#ifdef dynamic_linking
	    hLib = LoadLibrary(_T("ws2_32.dll"));
        if (hLib == 0)
			throw std::exception("Unable to load ws2_32.dll");
        (FARPROC &)accept = GetProcAddress(hLib, "accept");
        if (!accept)
			throw std::exception("Unable to load accept function");
        (FARPROC &)bind = GetProcAddress(hLib, "bind");
        if (!bind)
            throw std::exception("Unable to load bind function");
        (FARPROC &)closesocket = GetProcAddress(hLib, "closesocket");
        if (!closesocket)
            throw std::exception("Unable to load closesocket function");
        (FARPROC &)htons = GetProcAddress(hLib, "htons");
        if (!htons)
            throw std::exception("Unable to load htons function");
        (FARPROC &)htonl = GetProcAddress(hLib, "htonl");
        if (!htonl)
            throw std::exception("Unable to load htonl function");
        (FARPROC &)listen = GetProcAddress(hLib, "listen");
        if (!listen)
            throw std::exception("Unable to load listen function");
        (FARPROC &)recv = GetProcAddress(hLib, "recv");
        if (!recv)
            throw std::exception("Unable to load recv function");
        (FARPROC &)select = GetProcAddress(hLib, "select");
        if (!select)
            throw std::exception("Unable to load select function");
        (FARPROC &)send = GetProcAddress(hLib, "send");
        if (!send)
            throw std::exception("Unable to load send function");
        (FARPROC &)socket = GetProcAddress(hLib, "socket");
        if (!socket)
            throw std::exception("Unable to load socket function");
        (FARPROC &)WSACleanup = GetProcAddress(hLib, "WSACleanup");
        if (!WSACleanup)
            throw std::exception("Unable to load WSACleanup function");
        (FARPROC &)WSAGetLastError = GetProcAddress(hLib, "WSAGetLastError");
        if (!WSAGetLastError)
            throw std::exception("Unable to load WSAGetLastError function");
        (FARPROC &)WSAStartup = GetProcAddress(hLib, "WSAStartup");
        if (!WSAStartup)
            throw std::exception("Unable to load WSAStartup function");
#endif
	}
public:
    static Net* work()
	{
        if (instance == NULL)
            instance = new Net();
        return instance;
    }

    int Accept AcceptParams
	{
        return accept(s, addr, addrlen);
    }

    int Bind BindParams
	{
        return bind(s, name, namelen);
    }
    
    int CloseSocket CloseSocketParams
	{
        return closesocket(s);
    }

    unsigned short Htons HtonsParams
	{
        return htons(hostshort);
    }
    
    unsigned short Htonl HtonlParams
	{
        return htonl(hostlong);
    }

    int Listen ListenParams
	{
        return listen(s, backlog);
    }

    int Recv RecvParams
	{
        return recv(s, buf, len, flags);
    }

    int Select SelectParams
	{
        return select(nfds, readfds, writefds, exceptfds, timeout);
    }

    int Send SendParams
	{
        return send(s, buf, len, flags);
    }

    SOCKET Socket SocketParams
	{
        return socket(af, type, protocol);
    }

    int NWSACleanup WSACleanupParams
	{
        return WSACleanup();
    }
    
    int NWSAGetLastError WSAGetLastErrorParams
	{
        return WSAGetLastError();
    }

    int NWSAStartup WSAStartupParams
	{
        return WSAStartup(wVersionRequested, lpWSAData);
    }
};
