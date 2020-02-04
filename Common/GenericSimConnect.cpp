#include "GenericSimConnect.h"



GenericSimConnect::GenericSimConnect(std::shared_ptr<AsyncQueue<std::string>> const& data_queue,
 
	std::shared_ptr<AsyncQueue<std::string>> const& msg_queue, std::string message) :data_queue_(data_queue), msg_queue_(msg_queue)
{
	ONE_SECOND = 1000 / UPDATE_RATE;
	current_cycle = ONE_SECOND;

	if (msg_queue_->available()) {
		msg_queue_->push(message);
	}
	// start update thread
	timer_thread = std::thread([this]
		{
			this->timer();
		});
}


void GenericSimConnect::destroy()
{
	if (!timer_has_stopped) {
		// stop the update
		std::unique_lock<std::mutex> lk(timer_done_mutex);
		{
			std::lock_guard<std::mutex> lock(sim_timer_mutex);
			stop_timer = true;
		}
		condition.wait(lk, [this]
			{
				return (this->timer_has_stopped);
			});
	}

	if (timer_thread.joinable())
	{
		timer_thread.join();
	}


	delete this;
}


bool GenericSimConnect::is_connected()
{
	return dataService.isConnected();
}


bool GenericSimConnect::start()
{
	enable_sim_connect = true;
	sim_connect_is_open = dataService.open(msg_queue_);

	return sim_connect_is_open;
}

void GenericSimConnect::timer()
{
	current_cycle = ONE_SECOND;
	stop_timer = false;
	timer_has_stopped = false;
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::duration<double, std::milli> elapsed;
	//
	//

	while (true) {

		start = std::chrono::high_resolution_clock::now();

		// do we need to shut down
		{
			std::lock_guard<std::mutex> lock(sim_timer_mutex);
			if (stop_timer)
			{
				break;
			}
		}
		if (enable_sim_connect) {
			// do work then sleep
			// --
			if (sim_connect_is_open)
			{
				ProcessResult process_result = dataService.process(data_queue_, msg_queue_, all_data_requested);
				sim_connect_is_open = process_result != ProcessResult::failed;
				if (process_result == ProcessResult::initialized && all_data_requested) {
					all_data_requested = false;
				}
			}
		}
		elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);

		// sleep
		auto elapsedCount = elapsed.count();
		long long wait = long long(UPDATE_RATE - elapsedCount);

		if (wait > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(wait));

		}

		// update counters
		if (current_cycle >= ONE_SECOND)
		{
			current_cycle = 0;
			if (enable_sim_connect) {
				sim_connect_is_open = dataService.open(msg_queue_);
			}
		}
		else
		{
			current_cycle++;
		}

	}


	//
	// update  has stopped
	//
	std::unique_lock<std::mutex> lk(timer_done_mutex);
	timer_has_stopped = true;
	lk.unlock();
	condition.notify_one();

}



