#include "CustomSocket.h"
#include "Network.h"
#include <list>
#include <string>
#include <stdarg.h>
#include <strsafe.h>
#include <time.h>


//Static members init
const int CustomSocket::ProtocolVersion = 1;
const int CustomSocket::MilliSecondsToWait = 300;
const int CustomSocket::ListenerPort = 21307;
const bool CustomSocket::IsTracing = false;


CustomSocket::CustomSocket()
{
    this->initialized = true;
    if (this->InitSocketsAndAcceptConnection() != 0)
    {
        this->WriteToLog("Error while client connection  (CustomSocket::CustomSocket())");
        this->initialized = false;
    }
}


int CustomSocket::InitSocketsAndAcceptConnection()
{
    try
    {
        Sleep(200);
        char buff[1024];
        sockaddr_in local_sin;
		if (Net::work()->NWSAStartup(0x202,(WSADATA*)&buff[0]))
        {
            WriteToLog("WSA start error #%d  (CustomSocket::InitSocketsAndAcceptConnection())", Net::work()->NWSAGetLastError());
            return 1;
        }
        SOCKET socket_listener;
		if ((socket_listener = Net::work()->Socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
        {
            this->WriteToLog("Socket creation WSA error #%d  (CustomSocket::InitSocketsAndAcceptConnection())", Net::work()->NWSAGetLastError());
            Net::work()->NWSACleanup();
            return 1;
        }
        local_sin.sin_family = AF_INET;
		local_sin.sin_port = Net::work()->Htons(CustomSocket::ListenerPort);  
		local_sin.sin_addr.s_addr = Net::work()->Htonl(INADDR_ANY);
        if (Net::work()->Bind(socket_listener, (sockaddr*) &local_sin, sizeof (local_sin)) == SOCKET_ERROR)
        {
            this->WriteToLog("Socket bind WSA error #%d  (CustomSocket::InitSocketsAndAcceptConnection())", Net::work()->NWSAGetLastError());
            Net::work()->NWSACleanup();
            return 1;
        }
		if(Net::work()->Listen(socket_listener,0x100))
        {
            this->WriteToLog("Socket listen WSA error #%d  (CustomSocket::InitSocketsAndAcceptConnection())", Net::work()->NWSAGetLastError());
			Net::work()->CloseSocket(socket_listener);
            Net::work()->NWSACleanup();
            return 1;
        }
        sockaddr_in client_addr;
        int client_addr_size=sizeof(client_addr);	
        
        if (CustomSocket::IsTracing) {
            this->WriteToLog("Ready to accept connections"); }

		commands_socket=Net::work()->Accept(socket_listener, (sockaddr*)&client_addr, &client_addr_size);
        
        if (CustomSocket::IsTracing) { 
            this->WriteToLog("Socket connection accepted  (CustomSocket::InitSocketsAndAcceptConnection())"); }

		Net::work()->CloseSocket(socket_listener);
        return 0;
    }
    catch(...)
    {
        this->WriteToLog("Unexpected exception occured  (CustomSocket::InitSocketsAndAcceptConnection())");
        this->initialized = false;
    }
    return 1;
}


bool CustomSocket::IsInitialized()
{
    return this->initialized;
}


Message* CustomSocket::ReceiveMessage()
{
    time_t beginningTime;
    time(&beginningTime);
    Message* message = new Message;
    try
    {
        while(true)
        {
            time_t currentTime;
            time(&currentTime);
            if (difftime(currentTime, beginningTime)*1000 > CustomSocket::MilliSecondsToWait)
                break;

            if (this->IsIncomingData())
            {
                if (this->ReadMessageFromSocket(*message) == 0)
                    return message;
            }
            Sleep(50);
        }
        if (CustomSocket::IsTracing)
            this->WriteToLog("Have been waiting for the message for too long  CustomSocket::ReceiveMessage()");
    }
    catch (...)
    {
        this->WriteToLog("Unexpected exception occured  (CustomSocket::ReceiveMessage())");
        this->initialized = false;
    }
    delete message;
    return NULL;
}


int CustomSocket::IsIncomingData()
{
    fd_set read,write,except;
    read.fd_count=1;
    read.fd_array[0]=commands_socket;
    write.fd_count=0;
    except.fd_count=0;
    
    timeval tm;
    tm.tv_sec=0;
    tm.tv_usec=0;

	int res=Net::work()->Select(0,&read,&write,&except,&tm);

    if (res==SOCKET_ERROR)
    {
        this->WriteToLog("Socket error  (CustomSocket::IsIncomingData())");
        this->initialized = false;
        return false;
    }
    return res == 0 ? 0 : 1;
}


int CustomSocket::ReadMessageFromSocket(Message &msg)
{
    try
    {
        char buff[1024];
		int msize=Net::work()->Recv(commands_socket,&buff[0],sizeof(buff)-1,0);
        
        if (CustomSocket::IsTracing) {
            this->WriteToLog("Received message, size: %d", msize);	}	
        
        return msg.Parse(buff, msize) ? 0 : 1;
    }
    catch(...)
    {
        this->WriteToLog("Unexpected exception occured  CustomSocket::ReadMessageFromSocket()");
    }
    return 1;
}


int CustomSocket::SendMessage(Message message)
{
    try
    {
        if (this->initialized && this->commands_socket)
        {
			Net::work()->Send(this->commands_socket, message.GetBytes(), message.GetSize(), 0);
        }
        return 0;
    }
    catch(...)
    {
        this->WriteToLog("Error sending message  (CustomSocket::SendMessage())");
    }
    return 1;
}


CustomSocket::~CustomSocket()
{
    try
    {
		Net::work()->CloseSocket(commands_socket);
        Net::work()->NWSACleanup();
        this->initialized = false;
    }
    catch(...)
    {
        this->WriteToLog("Excepiton occured in the destructor  (CustomSocket::~CustomSocket())");
    }
}


void CustomSocket::WriteToLog(LPCSTR message, ...)
{
    char buf[1024];
    char buf0[1024];
    char buf1[32];
    char buf2[32];
    _strtime_s(buf1, 32);	
	_strdate_s(buf2, 32);
    va_list args;
    va_start(args, message);
    StringCbVPrintfA(buf, 1024, message, args);
    sprintf_s(buf0, "%s %s %s\n", buf1, buf2, buf);
    printf(buf0);
}