#ifndef CLIENT_H
#define CLIENT_H

#include <mutex>


#include <atomic>
#include <vector>
#include "SDK.h"
#include <nlohmann/json.hpp>


void iFly737NG_SendMessageInt(int Command, int value);

unsigned int registerWindowiFly737NG_MSG_SDK();
bool open_Ifly737();
char* openSimDataFileMapping();
char* openSimDataFileMapping2();
void iFly737NG_SendMessageInt(int Command, int value);
void sendMessageInt(int cmd, int value);
bool init_Ifly_send_message();


class Ifly737
{
public:


	~Ifly737()
	{
	}

	static Ifly737* instance()
	{
		static std::mutex mutex;
		if(!iflyInstance)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if(!iflyInstance)
			{
				iflyInstance = new Ifly737();
			}
		}

		return iflyInstance;
	}

	static void drop()
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);
		delete iflyInstance;
		iflyInstance = 0;
	}


	bool get_ifly737_data();
	std::string process737();

	
private:
	Ifly737();

	static Ifly737* iflyInstance;
	std::string buildJsonIfly737();

	std::string initialize();


	static bool initialized;
	static bool initialization_requested;



public:
	static void requestInitialization() { initialization_requested = true; }

	static struct ShareMemorySDK* shareMemSDK;

	char* simData;
	bool iflyIsValid;
	ShareMemorySDK ngxData;
};

#endif // CLIENT_H
