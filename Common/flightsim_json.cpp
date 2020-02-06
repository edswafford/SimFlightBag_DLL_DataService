

#include <string>
#include "flightsim_json.h"

using json = nlohmann::json;

bool has_changed(const double a, const double b, const double epsilon)
{
	return fabs(a - b) > epsilon;
}


Flightsim_json::Flightsim_json()
{
	gen_data.zulu_time = -1.0;
}

std::string Flightsim_json::build_json(const GeneralSimData& current)
{
	json js;

	if (!position_is_close(gen_data.longitude, current.longitude) || !position_is_close(gen_data.latitude, current.latitude) ||
		has_changed(gen_data.zulu_time, current.zulu_time, 60.0) || gen_data.zulu_time < 0.0)
	{

		gen_data.longitude = current.longitude;
		gen_data.latitude = current.latitude;
		gen_data.headingTrue = current.headingTrue;
		gen_data.headingTrue = current.headingMagnetic;

		gen_data.zulu_time = current.zulu_time;

		js["messageType"] = "genAircraft";
		js["AircraftData"] = {
			{"longitude", gen_data.longitude},
			{"latitude", gen_data.latitude},
			{"headingTrue", current.headingTrue},
			{"headingMagnetic", current.headingMagnetic},
			{"zuluTime", current.zulu_time},
		};
	}
	return js.dump();
}

std::string Flightsim_json::build_json_737ngx(const GeneralSimData& current, const bool initialized)
{
	json js;
	json json_string;
	if(initialized)
	{
		gen_data.throttle_lever_1_precentage = current.throttle_lever_1_precentage;
		gen_data.throttle_lever_2_precentage = current.throttle_lever_2_precentage;
		json_string["throttle_lever_1"] = gen_data.throttle_lever_1_precentage;
		json_string["throttle_lever_2"] = gen_data.throttle_lever_2_precentage;

		gen_data.rev_1_precentage = current.rev_1_precentage;
		gen_data.rev_2_precentage = current.rev_2_precentage;
		json_string["rev_1"] = gen_data.rev_1_precentage;
		json_string["rev_2"] = gen_data.rev_2_precentage;
		if (json_string.size() > 0)
		{
			js["messageType"] = "737ngx";
			js["Initialize"] = json::parse(json_string.dump());
		}
	}
	else {

		if (has_changed(gen_data.throttle_lever_1_precentage, current.throttle_lever_1_precentage, 4.0) )
		{
			gen_data.throttle_lever_1_precentage = current.throttle_lever_1_precentage;
			json_string["throttle_lever_1"] = gen_data.throttle_lever_1_precentage;
		}
		if (has_changed(gen_data.throttle_lever_2_precentage, current.throttle_lever_2_precentage, 4.0))
		{
			gen_data.throttle_lever_2_precentage = current.throttle_lever_2_precentage;
			json_string["throttle_lever_2"] = gen_data.throttle_lever_2_precentage;
		}

		if (has_changed(gen_data.rev_1_precentage, current.rev_1_precentage, 0.1) )
		{
			gen_data.rev_1_precentage = current.rev_1_precentage;
			json_string["rev_1"] = gen_data.rev_1_precentage;
		}
		if (has_changed(gen_data.rev_2_precentage, current.rev_2_precentage, 0.1))
		{
			gen_data.rev_2_precentage = current.rev_2_precentage;
			json_string["rev_2"] = gen_data.rev_2_precentage;
		}
		if (json_string.size() > 0)
		{
			js["messageType"] = "737ngx";
			js["AircraftChanges"] = json::parse(json_string.dump());

		}
	}

	return js.dump();
}


