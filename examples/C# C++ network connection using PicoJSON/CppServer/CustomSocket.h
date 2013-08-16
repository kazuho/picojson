#pragma once

#include "MessageAndSearilization.h"
#define _WINSOCKAPI_
#include <WS2tcpip.h>
#include <Windows.h>


#ifdef SendMessage
#undef SendMessage
#endif


class CustomSocket
{
public:	
	CustomSocket();
	int InitSocketsAndAcceptConnection();
	bool IsInitialized();
	int SendMessage(Message client_command);
	Message* ReceiveMessage();
	void WriteToLog(LPCSTR message, ...);
	virtual ~CustomSocket();

	static const int ProtocolVersion;
	static const int MilliSecondsToWait;
	static const int ListenerPort;
	static const bool IsTracing;
	
private:
    bool initialized;
	int ReadMessageFromSocket(Message &msg);
	int IsIncomingData();

	UINT_PTR commands_socket;
};