#pragma once
#include <nlohmann/json.hpp>
#include "GeneralSimData.h"


class Flightsim_json
{
public:
	Flightsim_json();

	const double position_epsilon = 1.0 / 111000.0;
	bool position_is_close(const double a, const double b) const
	{
		return fabs(a - b) < position_epsilon;
	}

	bool has_changed(const double a, const double b, const double epsilon) const
	{
		return fabs(a - b) > epsilon;
	}

	std::string build_json(const GeneralSimData& current);

	std::string build_json_737ngx(const GeneralSimData & current, bool initialize = false);

private:
	GeneralSimData gen_data;

};