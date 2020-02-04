#pragma once
#include <vector>
#include <string>
#include <Windows.h>

#include "aysnc_queue.h"
#include "flightsim_json.h"
#include "Pmdg737ngx.h"


enum ProcessResult {
	connected,
	initialized,
	failed,
};


// Declare a structure to hold the send IDs and identification strings
struct  SimConnectRequest {
	int index;
	DWORD   sendid;
	std::string last_request;
};


class SimDataService {
public:
	SimDataService():is_connected(false){}
	~SimDataService();

	bool open(std::shared_ptr<AsyncQueue<std::string>> const& msg_queue);
	ProcessResult process(std::shared_ptr<AsyncQueue<std::string>> const& data_queue,
		std::shared_ptr<AsyncQueue<std::string>> const& msg_queue, bool& all_data_requested);
	void close();

	bool isConnected() { return is_connected; }

	bool data_initialized{ false };
	
	static SimConnectRequest findSendRecord(DWORD id);

	static std::vector<SimConnectRequest>requestRecords;
private:

	HRESULT addSendRecord(const int index, const std::string);

	std::string aircraft_title;

	bool is_connected;
	Flightsim_json  flightsim_json;
	B737ngx b737ngx;

};

