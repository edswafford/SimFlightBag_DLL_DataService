#pragma once

#include <map>
#include "ISimDataService.h"
#include "SimDataService.h"


class GenericSimConnect : public ISimDataService
{
public:
	GenericSimConnect(std::shared_ptr<AsyncQueue<std::string>> const& data_queue,

		std::shared_ptr<AsyncQueue<std::string>> const& msg_queue, std::string message);

	void destroy();
	bool is_connected();
	void request_all_data() { all_data_requested = true; };
	bool start();

	//
	// Used by timer
	static const int HZ_40 = 25;
	static const int HZ_20 = 50; // 50ms
	static const int HZ_10 = 100; // 100ms
	static const int HZ_5 = 200; // 200ms
	static const int HZ_1 = 1000; // 1000ms

private:
	std::shared_ptr<AsyncQueue<std::string>> const& msg_queue_;
	std::shared_ptr<AsyncQueue<std::string>> const& data_queue_;

	SimDataService dataService;

	void timer();
	std::thread timer_thread;

	bool enable_sim_connect{ false };
	bool sim_connect_is_open{ false };
	bool stop_timer{ true };
	bool timer_has_stopped{ true };
	bool all_data_requested{ false };
	unsigned current_cycle;
	unsigned ONE_SECOND;
	unsigned UPDATE_RATE{ HZ_20 };

	std::mutex sim_timer_mutex;
	std::mutex timer_done_mutex;
	std::condition_variable condition;

};