
#include <map>
#include <memory>

#ifdef PMDG_737_NG3 
#include "PMDG_NG3_SDK.h"
#else
#include <PMDG_NGX_SDK.h>
#endif
#include "GeneralSimData.h"

#include <windows.h>
#include <tchar.h>

#include "SimConnect.h"
#include <strsafe.h>
//#include "aysnc_queue.h"
#include "SimDataService.h"

using json = nlohmann::json;


// static
std::vector<SimConnectRequest>SimDataService::requestRecords;



void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);
extern std::map< SIMCONNECT_EXCEPTION, std::string> SimConnectErrorMap;


enum EVENT_ID
{
	EVENT_SIM_START,
	EVENT_SIM_STOP,
	EVENT_PAUSE,
};

enum DATA_DEFINE_ID
{
	DEFINITION_1,
};

enum DATA_REQUEST_ID
{
	REQUEST_1,
	DATA_REQUEST,
};

enum PAUSESTATE
{
	UNPAUSED = 0,
	PAUSED
};


// Global Data used by Callback
std::string message;
GeneralSimData commonGeneralSimData;
AIRCRAFT_DATA_TYPE commonPmdg737ngx;

HANDLE hSimConnect = nullptr;
bool freshGeneralData = false;
bool freshPmdg737Data = false;
bool quitSimConnect = false;

bool sim_is_running = false;
bool simIsPaused = false;

SimDataService::~SimDataService()
{
	close();
}

void SimDataService::close()
{
	if (hSimConnect != nullptr)
	{
		is_connected = false;
		SimConnect_Close(hSimConnect);
		hSimConnect = nullptr;
	}
}

bool SimDataService::open(std::shared_ptr<AsyncQueue<std::string>> const& msg_queue)
{
	if (hSimConnect == nullptr) {
		is_connected = false;
		HRESULT hr = SimConnect_Open(&hSimConnect, "Request Data", nullptr, 0, 0, 0);

		if (SUCCEEDED(hr))
		{
			is_connected = true;
			quitSimConnect = false;
			msg_queue->push(std::string("SimConnect Running"));

			// Set up the data definition, but do not yet do anything with it
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Title", "", SIMCONNECT_DATATYPE_STRING256) != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition title"));
			}
			addSendRecord(0, "DEFINITION_1, 'Title'");

			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Latitude", "degrees") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition Plane Latitude"));
			}
			addSendRecord(1, "DEFINITION_1, 'Plane Latitude'");

			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Longitude", "degrees") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition Plane Longitude"));
			}
			addSendRecord(2, "DEFINITION_1, 'Plane Longitude'");

			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Heading Degrees True", "degrees") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition Plane heading true"));
			}
			addSendRecord(3, "DEFINITION_1, 'Plane Heading Degrees True'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Heading Degrees Magnetic", "degrees") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition Plane heading magnetic"));
			}
			addSendRecord(4, "DEFINITION_1, 'Plane Heading Degrees Magnetic'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "SPOILERS LEFT POSITION", "position") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition left spoiler"));
			}
			addSendRecord(5, "DEFINITION_1, 'SPOILERS LEFT POSITION'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "SPOILERS RIGHT POSITION", "position") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition right spoiler"));
			}
			addSendRecord(6, "DEFINITION_1, 'SPOILERS RIGHT POSITION'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ELEVATOR TRIM PCT", "Percent") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition Elevator trim"));
			}
			addSendRecord(7, "DEFINITION_1, 'ELEVATOR TRIM PCT'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:1", "Percent") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition throttle lever 1"));
			}
			addSendRecord(8, "DEFINITION_1, 'GENERAL ENG THROTTLE LEVER POSITION:1'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GENERAL ENG THROTTLE LEVER POSITION:2", "Percent") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition throttle lever 2"));
			}
			addSendRecord(9, "DEFINITION_1, 'GENERAL ENG THROTTLE LEVER POSITION:2'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GROUND VELOCITY", "Knots") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition ground speed"));
			}
			addSendRecord(10, "DEFINITION_1, 'GROUND VELOCITY'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "TURB ENG REVERSE NOZZLE PERCENT:1", "Percent") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition engine reverser 1"));
			}
			addSendRecord(11, "DEFINITION_1, 'TURB ENG REVERSE NOZZLE PERCENT:1'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "TURB ENG REVERSE NOZZLE PERCENT:2", "Percent") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed to add Data Definition engine reverser 2"));
			}
			addSendRecord(12, "DEFINITION_1, 'TURB ENG REVERSE NOZZLE PERCENT:1'");
			if (SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ZULU TIME", "Seconds") != S_OK)
			{
				msg_queue->push(std::string("SimConnect failed to add Data Definition ZULU time"));
			}
			addSendRecord(13, "DEFINITION_1, 'ZULU Time'");
 



			// Set up Request
			if (SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1,
				SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME) != S_OK)
			{
				msg_queue->push(std::string( "SimConnect Failure: Set up Request"));
			}
			addSendRecord(100, " 'SimConnect_RequestDataOnSimObject'");

			if (SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed simStart Subscription"));
			}
			addSendRecord(101, "   'EVENT_SIM_START'");
			if (SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_STOP, "SimStop") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed SimStop Subscription"));
			}
			addSendRecord(102, " 'EVENT_SIM_STOP'");
			if (SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_PAUSE, "Pause") != S_OK)
			{
				msg_queue->push(std::string( "SimConnect failed Pause Subscription"));
			}
			addSendRecord(103, " 'EVENT_PAUSE'");
			
			// Set up PMDG
			 // Associate an ID with the PMDG data area name
			if(SimConnect_MapClientDataNameToID(hSimConnect, PMDG_NGX_DATA_NAME, PMDG_NGX_DATA_ID) != S_OK)
			{
				msg_queue->push(std::string("SimConnect failed to map PMDG data"));
			}
			addSendRecord(104, " 'SimConnect_MapClientDataNameToID'");
			
			// Define the data area structure - this is a required step
			if(SimConnect_AddToClientDataDefinition(hSimConnect, PMDG_NGX_DATA_DEFINITION, 0, sizeof(AIRCRAFT_DATA_TYPE), 0, 0) != S_OK)
			{
				msg_queue->push(std::string("SimConnect failed to add PMDG definitions"));
			}
			addSendRecord(105, " 'PMDG_NGX_DATA_DEFINITION'");
			
			// Sign up for notification of data change.  
			// SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED listViewDeleteFlag asks for the data to be sent only when some of the data is changed.
		    if(SimConnect_RequestClientData(hSimConnect, PMDG_NGX_DATA_ID, DATA_REQUEST, PMDG_NGX_DATA_DEFINITION,
				SIMCONNECT_CLIENT_DATA_PERIOD_VISUAL_FRAME, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED, 0, 0, 0) != S_OK)
			{
				msg_queue->push(std::string("SimConnect failed request PMDG data"));
			}
			addSendRecord(106, " 'SimConnect_RequestClientData'");
			

		}
	}
	return is_connected;
}

ProcessResult SimDataService::process(std::shared_ptr<AsyncQueue<std::string>> const& data_queue,
	std::shared_ptr<AsyncQueue<std::string>> const& msg_queue, bool& all_data_requested)
{
	ProcessResult process_result = ProcessResult::connected;

	if (all_data_requested) {
		B737ngx::requestInitialData();
	}

	if (hSimConnect != nullptr) {
		SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, nullptr);

		if (freshGeneralData)
		{
			const auto title = std::string(commonGeneralSimData.title);
			if (aircraft_title != title)
			{
				if (msg_queue->available()) {
					aircraft_title = title;
					json js = { {"title", title} };
					msg_queue->push(js.dump());
				}
			}
			if (data_queue->available()) {

				const std::string json_string = flightsim_json.build_json(commonGeneralSimData);
				if (json_string != "null") {
					data_queue->push(json_string);
				}
				if (data_initialized) {
					const std::string json_737ngx = flightsim_json.build_json_737ngx(commonGeneralSimData);
					if (json_737ngx != "null")
					{
						data_queue->push(json_737ngx);
					}
				}
			}
			freshGeneralData = false;
		}
		if (freshPmdg737Data)
		{
			if (data_queue->available()) {
				const std::string json_string = b737ngx.processPmdg737(commonPmdg737ngx);
				if (!json_string.empty() && json_string != "null") {
					if (!data_initialized)
					{
						auto js = nlohmann::json::parse(json_string);
						if (js.find("Initialize") != js.end())
						{
							const std::string json_737ngx = flightsim_json.build_json_737ngx(commonGeneralSimData, true);
							if (json_737ngx != "null")
							{
								auto js_737ngx = nlohmann::json::parse(json_737ngx);
								auto js_init_737 = js_737ngx["Initialize"];
								auto js_init = js["Initialize"];
								for (auto& item : js_init_737.items()) {
									js_init.push_back({ item.key(), item.value() });
								}
								js["Initialize"] = js_init;
								data_queue->push(js.dump());
							}
							else {
								data_queue->push(json_string);
							}
							data_initialized = true;
							process_result = ProcessResult::initialized;

						}
					}
					else {
						data_queue->push(json_string);
					}
				}
			}
			freshPmdg737Data = false;
		}
		else if (all_data_requested) {
			if (data_queue->available()) {
				if (commonPmdg737ngx.AircraftModel >= 1 && commonPmdg737ngx.AircraftModel <= 7) {
					B737ngx::requestInitialData();
					const std::string json_string = b737ngx.processPmdg737(commonPmdg737ngx);
					if (!json_string.empty() && json_string != "null") {
						all_data_requested = false;

						auto js = nlohmann::json::parse(json_string);
						const std::string json_737ngx = flightsim_json.build_json_737ngx(commonGeneralSimData, true);
						if (json_737ngx != "null")
						{
							auto js_737ngx = nlohmann::json::parse(json_737ngx);
							auto js_init_737 = js_737ngx["Initialize"];
							auto js_init = js["Initialize"];
							for (auto& item : js_init_737.items()) {
								js_init.push_back({ item.key(), item.value() });
							}
							js["Initialize"] = js_init;
							data_queue->push(js.dump());
						}
						else {
							data_queue->push(json_string);
						}
					}
				}
			}
		}
		if (quitSimConnect)
		{
			SimConnect_Close(hSimConnect);
			hSimConnect = nullptr;
			is_connected = false;
			process_result = ProcessResult::failed;
		}
	}
	else {
		is_connected = false;
		process_result = ProcessResult::failed;
	}

	return   process_result;
}



HRESULT SimDataService::addSendRecord(const int index, const std::string text)
{
	DWORD id;
	int hr = SimConnect_GetLastSentPacketID(hSimConnect, &id);
	if (SUCCEEDED(hr)) {
		SimConnectRequest request;
		request.index = index;
		request.sendid = id;
		request.last_request = text;
		SimDataService::requestRecords.push_back(request);
	}
	return hr;
}

SimConnectRequest SimDataService::findSendRecord(DWORD id)
{
	for (auto& rec : SimDataService::requestRecords)
	{
		if (id == rec.sendid) {
			return rec;
		}
	}
	return SimConnectRequest{ -1, 0, "" };
}


void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_CLIENT_DATA: // Receive and process the NGX data block
	{
		SIMCONNECT_RECV_CLIENT_DATA *pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;

		switch (pObjData->dwRequestID)
		{
		case DATA_REQUEST:
		{
			AIRCRAFT_DATA_TYPE *pS = (AIRCRAFT_DATA_TYPE*)&pObjData->dwData;
			memcpy_s(&commonPmdg737ngx, sizeof(commonPmdg737ngx), pS, sizeof(AIRCRAFT_DATA_TYPE));
			freshPmdg737Data = true;
			break;
		}
		}
		break;
	}

	case SIMCONNECT_RECV_ID_EVENT:
	{
		SIMCONNECT_RECV_EVENT* evt = static_cast<SIMCONNECT_RECV_EVENT*>(pData);

		switch (evt->uEventID)
		{
		case EVENT_SIM_START:
			sim_is_running = true;
			message = "Simconnect: Sim Start";
			break;

		case EVENT_SIM_STOP:
			sim_is_running = false;
			message = "Simconnect: Sim Stop";
			break;

		case EVENT_PAUSE:
			switch (evt->dwData)
			{
			case UNPAUSED:
				message = "SimConnect Unpaused";
				simIsPaused = false;
				break;

			case PAUSED:
				message = "SimConnect Paused";
				simIsPaused = true;
				break;
			}
			break;

		default:
			break;
		}
		break;
	}


	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = static_cast<SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*>(pData);

		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
		{
			GeneralSimData* simDataPtr = reinterpret_cast<GeneralSimData*>(&pObjData->dwData);
			memcpy_s(&commonGeneralSimData, sizeof(commonGeneralSimData), simDataPtr, sizeof(commonGeneralSimData));

			freshGeneralData = true;


			break;
		}
		default:
			break;
		}
		break;
	}


	case SIMCONNECT_RECV_ID_QUIT:
	{
		quitSimConnect = true;
		break;
	}

	case SIMCONNECT_RECV_ID_OPEN:
	{
		// enter code to handle SimConnect version information received in a SIMCONNECT_RECV_OPEN structure.
		SIMCONNECT_RECV_OPEN *openData = static_cast<SIMCONNECT_RECV_OPEN*>(pData);
		message = "SimConnect ver " + std::to_string(openData->dwSimConnectVersionMajor) + "." +
			std::to_string(openData->dwSimConnectVersionMinor) + " build " + std::to_string(openData->dwSimConnectBuildMajor) +
				"." + std::to_string(openData->dwSimConnectBuildMinor) +
			+ "\r\nApplication " +
		std::string(openData->szApplicationName) + " ver " + std::to_string(openData->dwApplicationVersionMajor) + "."
		+ std::to_string(openData->dwApplicationVersionMinor) + " build " + std::to_string(openData->dwApplicationBuildMajor)
		+ "." + std::to_string(openData->dwApplicationBuildMinor);
		break;
	}

	case SIMCONNECT_RECV_ID_EXCEPTION:
	{
		SIMCONNECT_RECV_EXCEPTION* except = static_cast<SIMCONNECT_RECV_EXCEPTION*>(pData);
		auto error = static_cast<SIMCONNECT_EXCEPTION>(except->dwException);
		auto it = SimConnectErrorMap.find(error);
		// Locate the bad call and print it out
		auto request = SimDataService::findSendRecord(except->dwSendID);

		if (it != SimConnectErrorMap.end()) {
			message = "SimConnect Exception # " + std::to_string(except->dwException) +
				std::string("\n") + it->second + std::string(" Last ID ") + request.last_request;
		}
//		if (request.index != 18) {
//			quitSimConnect = true;
//		}
		break;
	}

	default:
		message = "UNKNOWN DATA RECEIVED: pData= " + std::to_string(pData->dwID) + std::string(" cbData= ") + std::to_string(cbData);
		break;
	}
}
