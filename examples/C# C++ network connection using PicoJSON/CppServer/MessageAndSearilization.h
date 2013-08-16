#pragma once

#include "../../../picojson.h"
#include <sstream>
#include <string>
#include <iterator>



class Message
{

public:
    enum MessageType { ProtocolVersion, Action };

public:
    Message()
    {
        messageBuffer = NULL;
    }

    Message(Message::MessageType command, int argument = 0)
    {
        messageBuffer = NULL;
        CustomMessage = command;
        Argument = argument;
    }
    
    Message(Message::MessageType command, int argument, std::string stringMessage)
    {
        messageBuffer = NULL;
        CustomMessage = command;
        Argument = argument;
        StringMessage = stringMessage;
    }

public:
    MessageType CustomMessage;
    int Argument;
    std::string StringMessage;
    
private:
    char* messageBuffer;
    int size;

private:
    char* Serialize()
    {
        std::stringstream messageBuilder;
        messageBuilder << "{" << std::endl;
        messageBuilder << "\"CustomMessage\":" << (int)CustomMessage << "," << std::endl;
        messageBuilder << "\"Argument\":" << Argument << "," << std::endl;
        messageBuilder << "\"StringMessage\":\"" << StringMessage.c_str() << "\"" << std::endl;
        messageBuilder << "}";
        size = strlen(messageBuilder.str().c_str());
        if (messageBuffer != NULL)
            messageBuffer = (char*)realloc((void*)messageBuffer, size);
        else
            messageBuffer = (char*)malloc(size);
        memcpy((void*)messageBuffer, (void*)messageBuilder.str().c_str(), size);
        return messageBuffer;
    }

    bool Deserialize(void* data, int size)
    {
        try
        {
            std::istringstream buf((char*)data, size);
            picojson::value v;
            std::string err;
            err = picojson::parse(v, buf);
            if (err.empty() && v.is<picojson::object>())
            {
                if (v.contains("CustomMessage") && v.get("CustomMessage").is<int>())
                    CustomMessage = (MessageType) (int) v.get("CustomMessage").get<double>();
                if (v.contains("Argument") && v.get("Argument").is<double>())
                    Argument = (int) v.get("Argument").get<double>();
                if (v.contains("StringMessage") && v.get("StringMessage").is<std::string>())
                    StringMessage = v.get("StringMessage").get<std::string>();
                return true;
            }
        }catch(...) { }
        return false; 
    }

public:
    char* GetBytes()
    {
        return Serialize();
    }

    int GetSize()
    {
        Serialize();
        return size;
    }

    bool Parse (void* data, int size)
    {
        return Deserialize(data, size);
    }
};