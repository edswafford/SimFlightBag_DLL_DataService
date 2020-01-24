#pragma once
#ifdef PMDG_737_NG3 
#include "PMDG_NG3_SDK.h"
typedef  PMDG_NG3_Data AIRCRAFT_DATA_TYPE;
#define PMDG_NGX_DATA_NAME PMDG_NG3_DATA_NAME
#define PMDG_NGX_DATA_ID PMDG_NG3_DATA_ID
#define PMDG_NGX_DATA_DEFINITION PMDG_NG3_DATA_DEFINITION
#else
#include <PMDG_NGX_SDK.h>
typedef PMDG_NGX_Data AIRCRAFT_DATA_TYPE;
#endif

#include <cstring>
#include <nlohmann/json.hpp>

class B737ngx
{
public:
	std::string processPmdg737(const AIRCRAFT_DATA_TYPE& data);
	static void requestInitialData();
	static void clearInitialDataRequest();
	static bool initialDataRequested();
	static bool isInitialized();


private:
	std::string initialize(const AIRCRAFT_DATA_TYPE& data);
	std::string buildJsonPmdg737(const AIRCRAFT_DATA_TYPE& data);


	static bool initialized;
	static bool initialization_requested;
	AIRCRAFT_DATA_TYPE ngxData;
};
