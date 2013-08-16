#include "CustomSocket.h"
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include "conio.h"

using namespace std;

void SendData(CustomSocket customSocket)
{
    Message data(Message::MessageType::Action, 1, "Custom string");
    customSocket.SendMessage(data);
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;
    try
    {
        cout << "Program started. Waiting for the connection..." << endl;
        CustomSocket customSocket;
        if (customSocket.IsInitialized())
        {
            cout << "A client connected" << endl << endl;
            cout << "Waiting for the message containing protocol version" << endl; 
            Message* protocolVersion = customSocket.ReceiveMessage();
            int version = -1;
            if (protocolVersion != NULL)
            {
                cout << "Got message from the client" << endl;
                cout << "Client's protocol version: " << protocolVersion->Argument << endl;
				if (protocolVersion->CustomMessage == Message::ProtocolVersion && protocolVersion->Argument == 1)
				{
                    cout << "Protocol versions matched" << endl << endl;
                    cout << "Sending data to client" << endl << endl;
                    SendData(customSocket);
				}
                else
                    cout << "Protocol version mismatch. Expected: 1" << endl << endl;
                delete protocolVersion;
            }
            else
            {
                cout << "Didn't get client's protocol version" << endl << endl;
            }
        }
    }
    catch(...)
    {
        cout << "Unexpected exception  (_tmain)" << endl << endl;
        nRetCode = 1;
    }
    cout << "Execution finished. Press any key";
    _getch();
    return nRetCode;
}

