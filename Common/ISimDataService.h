#pragma once
#include <string>
#include "aysnc_queue.h"



class ISimDataService
{
public:
	virtual void destroy() = 0;

	virtual bool is_connected() = 0;

	virtual void request_all_data() = 0;

	virtual bool start() = 0;

};
