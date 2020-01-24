#ifndef _GENERAL_SIM_DATA_H
#define _GENERAL_SIM_DATA_H

#ifdef _SIM_FLIGHT_BAG_PROXY_PROJECT_
[
	uuid(a0d8ada3-f628-4a32-91be-81e9a387315a)
]
#endif

typedef struct GeneralSimData
{
	char title[256];
	double latitude;
	double longitude;
	double headingTrue;
	double headingMagnetic;

	double left_spoiler_position;
	double right_spoiler_position;
	double trim_precentage;
	double throttle_lever_1_precentage;
	double throttle_lever_2_precentage;
	double ground_speed;
	double rev_1_precentage;
	double rev_2_precentage;
	double zulu_time;
} GeneralSimData;
#endif