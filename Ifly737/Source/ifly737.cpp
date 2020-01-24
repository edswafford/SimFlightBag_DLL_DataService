#include <thread>

#include <windows.h>

#include "ifly737.h"
#include "logger/logger.h"

extern logger LOG;

using json = nlohmann::json;
bool Ifly737::initialization_requested = false;
bool Ifly737::initialized = false;


HWND findMSFS()
{
	DWORD error;
	HWND retval = FindWindowEx(NULL, NULL, "FS98MAIN", NULL);
	if (retval == NULL)
	{
		error = GetLastError();
	}
	FS_main_window = retval;

	return FS_main_window;
}



struct ShareMemorySDK* Ifly737::shareMemSDK = &ShareMemSDK;

Ifly737* Ifly737::iflyInstance = nullptr;




unsigned int registerWindowiFly737NG_MSG_SDK()
{
	DWORD error;
	unsigned int retVal = RegisterWindowMessageA("iFly737NG_MSG_SDK");
	if (retVal == 0)
	{
		error = GetLastError();
	}
	return retVal;
}
bool open_Ifly737()
{
	if (iFly737NG_MSGSDK == NULL)
	{
		unsigned int retVal = registerWindowiFly737NG_MSG_SDK();
		if (retVal == 0)
		{
			auto error = GetLastError();
		}
		iFly737NG_MSGSDK = retVal;
	}
	return iFly737NG_MSGSDK != 0;
}


char* openSimDataFileMapping()
{
	if (pBufSDK == NULL)
	{
		hMapFileSDK = OpenFileMapping(
			FILE_MAP_READ, // read/write access
			FALSE, // do not inherit the name
			szNameSDK); // name of mapping object 

		if (hMapFileSDK != NULL)
		{
			pBufSDK = (LPTSTR)MapViewOfFile(hMapFileSDK, // handle to mapping object
				FILE_MAP_READ, // read/write permission
				0,
				0,
				BUF_SIZE_SDK);
		}
	}
	return (char *)pBufSDK;
}


static void iFly737NG_SendMessageInt(int Command, int value)
{
	DWORD error;
	if (FS_main_window == nullptr) {
		if (findMSFS() != nullptr) {
			open_Ifly737();
		}

	}
	if (FS_main_window != NULL && iFly737NG_MSGSDK)
	{
		if (::PostMessageA(FS_main_window, iFly737NG_MSGSDK, Command, value) == 0)
		{
			error = GetLastError();
		}
	}
}

void sendMessageInt(int cmd, int value)
{
	iFly737NG_SendMessageInt(cmd, value);
}


bool init_Ifly_send_message()
{
	if (FS_main_window == nullptr) {
		findMSFS();
	}
	return FS_main_window != NULL && open_Ifly737();
}

std::string buildJsonIfly737()
{
	return std::string();
}



Ifly737::Ifly737()
{
	iflyIsValid = false;
	if (simData != nullptr) {
		UnmapViewOfFile(simData);
		pBufSDK = NULL;
		CloseHandle(hMapFileSDK);
		hMapFileSDK = NULL;
		simData = nullptr;
	}

}






bool Ifly737::get_ifly737_data()
{
	// set up shared memory and int Ifly737 if shared memory ptr is null
	if (simData == nullptr)
	{
		simData = openSimDataFileMapping();
		if (simData != nullptr) {
			memcpy(shareMemSDK, simData, sizeof(struct ShareMemorySDK));
			init_Ifly_send_message();
		}
	}


	if(simData != nullptr)
	{
		memcpy(shareMemSDK, simData, sizeof(struct ShareMemorySDK));

		iflyIsValid = (shareMemSDK->IFLY737NG_STATE == IFLY737NG_RUNNING);
	}
	else
	{
		iflyIsValid = false;
	}
	return iflyIsValid;
}


std::string Ifly737::process737()
{
	std::string json_string;
	if (!initialized || Ifly737::initialization_requested) {

		try {
			
			json_string = initialize();
			if (json_string != "null") {
				json js = {
					{"messageType", "737ngx"},
					{"Initialize", json::parse(json_string)}
				};

				Ifly737::initialization_requested = false;
				initialized = true;

				return js.dump();
			}
		}
		catch (std::exception e)
		{
			LOG() << "Failed to initial 737 NGX json data " << e.what();
		}


	}
	else if (initialized)
	{
		try {
			json_string = buildJsonIfly737();
			if (json_string != "null") {
				json js = {
					{"messageType", "737ngx"},
					{"AircraftChanges", json::parse(json_string)}
				};
				return js.dump();
			}
		}
		catch (std::exception e)
		{
			LOG() << "Failed to format 737 NGX json data " << e.what();
		}
	}

	return "";
}


std::string Ifly737::initialize(){
	bool valid = false;

ShareMemorySDK data;

	if (data.AircraftModel >= 1 && data.AircraftModel <= 7)
	{
		valid = true;
	}
	json js;
	js["DataIsValid"] = valid;
	js["OXY_Needle"] = data.Oxygen_Pressure;

	js["IRS_DisplaySelector"] = data.IRS_DisplaySelector;
	js["IRS_SysDisplay_R"] = data.IRS_SysDisplay_R;
	js["COMM_ServiceInterphoneSw"] = data.COMM_ServiceInterphoneSw != 0;
	js["OXY_SwNormal"] = data.Oxygen_Switch_Status != 0;

	js["FLTREC_SwNormal"] = !data.FLTREC_SwNormal != 0;

	js["FCTL_YawDamper_Sw"] = data.Yaw_Damper_Switches_Status != 0;
	js["FCTL_AltnFlaps_Control_Sw"] = data.Altn_Flap_Switches_Status != 0;
	js["FCTL_AltnFlaps_Sw_ARM"] = data.Altn_Flap_Master_Switches_Status != 0;
	js["NAVDIS_VHFNavSelector"] = data.VHF_NAV_Switches_Status;
	js["NAVDIS_IRSSelector"] = data.IRS_Switches_Status;
	js["NAVDIS_FMCSelector"] = data.FMC_Switches_Status;
	js["NAVDIS_SourceSelector"] = data..Source_Switches_Status;
	js["NAVDIS_ControlPaneSelector"] = data.Control_Panel_Switches_Status;
	js["FUEL_CrossFeedSw"] = data.Fuel_Crossfeed_Selector_Status != 0;
	js["ELEC_DCMeterSelector"] = data.DC_Meters_Selector_Status;
	js["ELEC_ACMeterSelector"] = data.AC_Meters_Selector_Status;
	js["ELEC_BatSelector"] = data.Battery_Switch_Status != 0;
	js["ELEC_CabUtilSw"] = data.CAB_UTIL_Switch_Status != 0;
	js["ELEC_IFEPassSeatSw"] = data.IFE_PASS_SEAT_Switches_Status != 0;
	js["ELEC_StandbyPowerSelector"] = data.STANDBY_POWER_Switch_Status;
	js["ELEC_GrdPwrSw"] = data.Ground_Power_Switches_Status != 0;
	js["ELEC_BusTransSw_AUTO"] = data.Bus_Transfer_Switches_Status != 0;
	js["OH_WiperLSelector"] = data.Wiper_L_Switches_Status;
	js["OH_WiperRSelector"] = data.Wiper_R_Switches_Status;
	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	js["LTS_OvereadPanelKnob"] = data.LTS_OvereadPanelKnob;
	js["AIR_EquipCoolingSupplyNORM"] = data.Equipment_COOLING_SUPPLY_Switch_Status != 0;
	js["AIR_EquipCoolingExhaustNORM"] = data.Equipment_COOLING_EXHAUST_Switch_Status != 0;
	js["LTS_EmerExitSelector"] = data.Emergency_Light_Switch_Status;
	js["COMM_NoSmokingSelector"] = data.COMM_NoSmokingSelector;
	js["COMM_FastenBeltsSelector"] = data.Fasten_Belts_Switches_Status;
	js["ICE_WingAntiIceSw"] = data.Wing_AntiIce_Switch_Status != 0;
	js["AIR_TempSourceSelector"] = data.AIR_TempSourceSelector;
	js["AIR_TrimAirSwitch"] = data.TRIM_AIR_Switch_Status != 0;
	js["AIR_APUBleedAirSwitch"] = data.APU_Bleed_Air_Switches_Status != 0;
	js["AIR_IsolationValveSwitch"] = data.Isolation_Valve_Switches_Status;
	js["AIR_OutflowValveSwitch"] = data.AIR_OutflowValveSwitch != 0;
	js["AIR_PressurizationModeSelector"] = data.Pressurization_Mode_Selector_Status;
	js["LTS_TaxiSw"] = data.Taxi_Light_Status != 0;
	js["APU_Selector"] = data.APU_Switches_Status;
	js["ENG_IgnitionSelector"] = data.Ignition_Select_Switches_Status;
	js["LTS_LogoSw"] = data.LTS_LogoSw != 0;
	js["LTS_PositionSw"] = data.Position_Light_Status != 0;
	js["LTS_AntiCollisionSw"] = data.Anti_Collision_Light_Status != 0;
	js["LTS_WingSw"] = data.Wing_Light_Status != 0;
	js["LTS_WheelWellSw"] = data.WHEEL_WELL_Light_Status != 0;
	js["MAIN_NoseWheelSteeringSwNORM"] = data.MAIN_NoseWheelSteeringSwNORM != 0;
	js["MAIN_LightsSelector"] = data.MAIN_LightsSelector != 0;
	js["MAIN_RMISelector1_VOR"] = data.MAIN_RMISelector1_VOR;
	js["MAIN_RMISelector2_VOR"] = data.MAIN_RMISelector2_VOR;
	js["MAIN_N1SetSelector"] = data.N1_Set_Status;
	js["MAIN_SpdRefSelector"] = data.N1_Set_Status;
	js["MAIN_FuelFlowSelector"] = data.Fuel_Flow_Status;
	js["MAIN_AutobrakeSelector"] = data.Autobrake_Switches_Status;
	js["MAIN_GearLever"] = data.Gear_Lever_Status;
	js["GPWS_FlapInhibitSw_NORM"] = data.Flap_Inhibit_Switches_Status != 0;
	js["GPWS_GearInhibitSw_NORM"] = data.Gear_Inhibit_Switches_Status != 0;
	js["GPWS_TerrInhibitSw_NORM"] = data.Terr_Inhibit_Switches_Status != 0;
	js["TRIM_StabTrimMainElecSw_NORMAL"] = data.Stab_Trim_Main_Elect_Switch_Status != 0;
	js["TRIM_StabTrimAutoPilotSw_NORMAL"] = data.Stab_Trim_Auto_Pilot_Switch_Status != 0;
	js["FIRE_DetTestSw"] = data.FIRE_DetTestSw != 0;
	js["FIRE_ExtinguisherTestSw"] = data.FIRE_ExtinguisherTestSw != 0;
	js["XPDR_XpndrSelector_2"] = data.XPDR_XpndrSelector_2;
	js["XPDR_AltSourceSel_2"] = data.XPDR_AltSourceSel_2;
	js["XPDR_ModeSel"] = data.XPDR_ModeSel;
	js["TRIM_StabTrimSw_NORMAL"] = data.Stabilizer_Trim_Switch_Status != 0;
	js["PED_FltDkDoorSel"] = data.PED_FltDkDoorSel != 0;
	js["IRS_aligned"] = data.IRS_aligned != 0;
	js["AircraftModel"] = data.AircraftModel;
	js["WeightInKg"] = data.WeightInKg;
	js["GPWS_V1CallEnabled"] = data.GPWS_V1CallEnabled != 0;
	js["GroundConnAvailable"] = data.GroundConnAvailable != 0;
	js["FMC_TakeoffFlaps"] = data.FMC_TakeoffFlaps;
	js["FMC_V1"] = data.FMC_V1;
	js["FMC_VR"] = data.FMC_VR;
	js["FMC_V2"] = data.FMC_V2;
	js["FMC_LandingFlaps"] = data.FMC_LandingFlaps;
	js["FMC_LandingVREF"] = data.FMC_LandingVREF;
	js["FMC_CruiseAlt"] = data.FMC_CruiseAlt;
	js["FMC_LandingAltitude"] = data.FMC_LandingAltitude;
	js["FMC_TransitionAlt"] = data.FMC_TransitionAlt;
	js["FMC_TransitionLevel"] = data.FMC_TransitionLevel;
	js["FMC_PerfInputComplete"] = data.FMC_PerfInputComplete != 0;
	js["FMC_DistanceToTOD"] = data.FMC_DistanceToTOD;
	js["FMC_DistanceToDest"] = data.FMC_DistanceToDest;
	js["ICE_WindowHeatTestSw"] = data.ICE_WindowHeatTestSw;
	js["IRS_ModeSelector_1"] = data.IRS_1_Switches_Status;
	js["IRS_ModeSelector_2"] = data.IRS_2_Switches_Status;
	js["ENG_EECSwitch_1"] = data.EEC_1_Switch_Status != 0;
	js["ENG_EECSwitch_2"] = data.EEC_2_Switch_Status != 0;
	js["FCTL_FltControl_Sw_1"] = data.Flight_Control_A_Switches_Status;
	js["FCTL_FltControl_Sw_2"] = data.Flight_Control_B_Switches_Status;
	js["FCTL_Spoiler_Sw_1"] = data.Spoiler_A_Switches_Status != 0;
	js["FCTL_Spoiler_Sw_2"] = data.Spoiler_B_Switches_Status != 0;
	js["FUEL_PumpFwdSw_1"] = data.Fuel_L_FWD_Switch_Status != 0;
	js["FUEL_PumpFwdSw_2"] = data.Fuel_R_FWD_Switch_Status != 0;
	js["FUEL_PumpAftSw_1"] = data.Fuel_L_AFT_Switch_Status != 0;
	js["FUEL_PumpAftSw_2"] = data.Fuel_R_AFT_Switch_Status != 0;
	js["FUEL_PumpCtrSw_1"] = data.Fuel_CENTER_L_Switch_Status != 0;
	js["FUEL_PumpCtrSw_2"] = data.Fuel_CENTER_L_Switch_Status != 0;
	js["ELEC_IDGDisconnectSw_1"] = data.ELEC_IDGDisconnectSw[0] != 0;
	js["ELEC_IDGDisconnectSw_2"] = data.ELEC_IDGDisconnectSw[1] != 0;
	js["ELEC_GenSw_1"] = data.ELEC_GenSw[0] != 0;
	js["ELEC_GenSw_2"] = data.ELEC_GenSw[1] != 0;
	js["ELEC_APUGenSw_1"] = data.ELEC_APUGenSw[0] != 0;
	js["ELEC_APUGenSw_2"] = data.ELEC_APUGenSw[1] != 0;
	js["ICE_WindowHeatSw_1"] = data.ICE_WindowHeatSw[0] != 0;
	js["ICE_WindowHeatSw_2"] = data.ICE_WindowHeatSw[1] != 0;
	js["ICE_WindowHeatSw_3"] = data.ICE_WindowHeatSw[2] != 0;
	js["ICE_WindowHeatSw_4"] = data.ICE_WindowHeatSw[3] != 0;
	js["ICE_ProbeHeatSw_1"] = data.ICE_ProbeHeatSw[0] != 0;
	js["ICE_ProbeHeatSw_2"] = data.ICE_ProbeHeatSw[1] != 0;
	js["ICE_EngAntiIceSw_1"] = data.ICE_EngAntiIceSw[0] != 0;
	js["ICE_EngAntiIceSw_2"] = data.ICE_EngAntiIceSw[1] != 0;
	js["HYD_PumpSw_eng_1"] = data.HYD_PumpSw_eng[0] != 0;
	js["HYD_PumpSw_eng_2"] = data.HYD_PumpSw_eng[1] != 0;
	js["HYD_PumpSw_elec_1"] = data.HYD_PumpSw_elec[0] != 0;
	js["HYD_PumpSw_elec_2"] = data.HYD_PumpSw_elec[1] != 0;
	js["AIR_RecircFanSwitch_1"] = data.AIR_RecircFanSwitch[0] != 0;
	js["AIR_RecircFanSwitch_2"] = data.AIR_RecircFanSwitch[1] != 0;
	js["AIR_PackSwitch_1"] = data.AIR_PackSwitch[0];
	js["AIR_PackSwitch_2"] = data.AIR_PackSwitch[1];
	js["AIR_BleedAirSwitch_1"] = data.AIR_BleedAirSwitch[0] != 0;
	js["AIR_BleedAirSwitch_2"] = data.AIR_BleedAirSwitch[1] != 0;
	js["LTS_LandingLtRetractableSw_1"] = data.LTS_LandingLtRetractableSw[0];
	js["LTS_LandingLtRetractableSw_2"] = data.LTS_LandingLtRetractableSw[1];
	js["LTS_LandingLtFixedSw_1"] = data.LTS_LandingLtFixedSw[0] != 0;
	js["LTS_LandingLtFixedSw_2"] = data.LTS_LandingLtFixedSw[1] != 0;
	js["LTS_RunwayTurnoffSw_1"] = data.LTS_RunwayTurnoffSw[0] != 0;
	js["LTS_RunwayTurnoffSw_2"] = data.LTS_RunwayTurnoffSw[1] != 0;
	js["ENG_StartSelector_1"] = data.ENG_StartSelector[0];
	js["ENG_StartSelector_2"] = data.ENG_StartSelector[1];
	js["EFIS_MinsSelBARO_1"] = data.EFIS_MinsSelBARO[0];
	js["EFIS_MinsSelBARO_2"] = data.EFIS_MinsSelBARO[1];
	js["EFIS_BaroSelHPA_1"] = data.EFIS_BaroSelHPA[0] != 0;
	js["EFIS_BaroSelHPA_2"] = data.EFIS_BaroSelHPA[1] != 0;
	js["MAIN_MainPanelDUSel_1"] = data.MAIN_MainPanelDUSel[0];
	js["MAIN_MainPanelDUSel_2"] = data.MAIN_MainPanelDUSel[1];
	js["MAIN_LowerDUSel_1"] = data.MAIN_LowerDUSel[0];
	js["MAIN_LowerDUSel_2"] = data.MAIN_LowerDUSel[1];
	js["MAIN_DisengageTestSelector_1"] = data.MAIN_DisengageTestSelector[0];
	js["MAIN_DisengageTestSelector_2"] = data.MAIN_DisengageTestSelector[1];
	js["FIRE_OvhtDetSw_1"] = data.FIRE_OvhtDetSw[0];
	js["FIRE_OvhtDetSw_2"] = data.FIRE_OvhtDetSw[1];
	js["FIRE_HandlePos_1"] = data.FIRE_HandlePos[0];
	js["FIRE_HandlePos_2"] = data.FIRE_HandlePos[1];
	js["FIRE_HandlePos_3"] = data.FIRE_HandlePos[2];
	js["FIRE_HandleIlluminated_1"] = data.FIRE_HandleIlluminated[0] != 0;
	js["FIRE_HandleIlluminated_2"] = data.FIRE_HandleIlluminated[1] != 0;
	js["FIRE_HandleIlluminated_3"] = data.FIRE_HandleIlluminated[2] != 0;
	js["CARGO_DetSelect_1"] = data.CARGO_DetSelect[0];
	js["CARGO_DetSelect_2"] = data.CARGO_DetSelect[1];
	js["CARGO_ArmedSw_1"] = data.CARGO_ArmedSw[0] != 0;
	js["CARGO_ArmedSw_2"] = data.CARGO_ArmedSw[1] != 0;
	js["ENG_StartValve_1"] = data.ENG_StartValve[0] != 0;
	js["ENG_StartValve_2"] = data.ENG_StartValve[1] != 0;
	js["COMM_SelectedMic_1"] = data.COMM_SelectedMic[0];
	js["COMM_SelectedMic_2"] = data.COMM_SelectedMic[1];
	js["COMM_SelectedMic_3"] = data.COMM_SelectedMic[2];
	js["COMM_ReceiverSwitches_1"] = data.COMM_ReceiverSwitches[0];
	js["COMM_ReceiverSwitches_2"] = data.COMM_ReceiverSwitches[1];
	js["COMM_ReceiverSwitches_3"] = data.COMM_ReceiverSwitches[2];
	js["IRS_annunGPS"] = data.IRS_annunGPS != 0;
	js["WARN_annunPSEU"] = data.WARN_annunPSEU != 0;
	js["OXY_annunPASS_OXY_ON"] = data.OXY_annunPASS_OXY_ON != 0;
	js["GEAR_annunOvhdLEFT"] = data.GEAR_annunOvhdLEFT != 0;
	js["GEAR_annunOvhdNOSE"] = data.GEAR_annunOvhdNOSE != 0;
	js["GEAR_annunOvhdRIGHT"] = data.GEAR_annunOvhdRIGHT != 0;
	js["FLTREC_annunOFF"] = data.FLTREC_annunOFF != 0;
	js["FCTL_annunYAW_DAMPER"] = data.FCTL_annunYAW_DAMPER != 0;
	js["FCTL_annunLOW_QUANTITY"] = data.FCTL_annunLOW_QUANTITY != 0;
	js["FCTL_annunLOW_PRESSURE"] = data.FCTL_annunLOW_PRESSURE != 0;
	js["FCTL_annunLOW_STBY_RUD_ON"] = data.FCTL_annunLOW_STBY_RUD_ON != 0;
	js["FCTL_annunFEEL_DIFF_PRESS"] = data.FCTL_annunFEEL_DIFF_PRESS != 0;
	js["FCTL_annunSPEED_TRIM_FAIL"] = data.FCTL_annunSPEED_TRIM_FAIL != 0;
	js["FCTL_annunMACH_TRIM_FAIL"] = data.FCTL_annunMACH_TRIM_FAIL != 0;
	js["FCTL_annunAUTO_SLAT_FAIL"] = data.FCTL_annunAUTO_SLAT_FAIL != 0;
	js["FUEL_annunXFEED_VALVE_OPEN"] = data.FUEL_annunXFEED_VALVE_OPEN != 0;
	js["ELEC_annunBAT_DISCHARGE"] = data.ELEC_annunBAT_DISCHARGE != 0;
	js["ELEC_annunTR_UNIT"] = data.ELEC_annunTR_UNIT != 0;
	js["ELEC_annunELEC"] = data.ELEC_annunELEC != 0;
	js["ELEC_annunSTANDBY_POWER_OFF"] = data.ELEC_annunSTANDBY_POWER_OFF != 0;
	js["ELEC_annunGRD_POWER_AVAILABLE"] = data.ELEC_annunGRD_POWER_AVAILABLE != 0;
	js["ELEC_annunAPU_GEN_OFF_BUS"] = data.ELEC_annunAPU_GEN_OFF_BUS != 0;
	js["APU_annunMAINT"] = data.APU_annunMAINT != 0;
	js["APU_annunLOW_OIL_PRESSURE"] = data.APU_annunLOW_OIL_PRESSURE != 0;
	js["APU_annunFAULT"] = data.APU_annunFAULT != 0;
	js["APU_annunOVERSPEED"] = data.APU_annunOVERSPEED != 0;
	js["AIR_annunEquipCoolingSupplyOFF"] = data.AIR_annunEquipCoolingSupplyOFF != 0;
	js["AIR_annunEquipCoolingExhaustOFF"] = data.AIR_annunEquipCoolingExhaustOFF != 0;
	js["LTS_annunEmerNOT_ARMED"] = data.LTS_annunEmerNOT_ARMED != 0;
	js["COMM_annunCALL"] = data.COMM_annunCALL != 0;
	js["COMM_annunPA_IN_USE"] = data.COMM_annunPA_IN_USE != 0;
	js["ICE_annunCAPT_PITOT"] = data.ICE_annunCAPT_PITOT != 0;
	js["ICE_annunL_ELEV_PITOT"] = data.ICE_annunL_ELEV_PITOT != 0;
	js["ICE_annunL_ALPHA_VANE"] = data.ICE_annunL_ALPHA_VANE != 0;
	js["ICE_annunL_TEMP_PROBE"] = data.ICE_annunL_TEMP_PROBE != 0;
	js["ICE_annunFO_PITOT"] = data.ICE_annunFO_PITOT != 0;
	js["ICE_annunR_ELEV_PITOT"] = data.ICE_annunR_ELEV_PITOT != 0;
	js["ICE_annunR_ALPHA_VANE"] = data.ICE_annunR_ALPHA_VANE != 0;
	js["ICE_annunAUX_PITOT"] = data.ICE_annunAUX_PITOT != 0;
	js["AIR_annunDualBleed"] = data.AIR_annunDualBleed != 0;
	js["AIR_annunRamDoorL"] = data.AIR_annunRamDoorL != 0;
	js["AIR_annunRamDoorR"] = data.AIR_annunRamDoorR != 0;
	js["AIR_FltAltWindow"] = data.AIR_FltAltWindow != 0;
	js["AIR_LandAltWindow"] = data.AIR_LandAltWindow != 0;
	js["WARN_annunFLT_CONT"] = data.WARN_annunFLT_CONT != 0;
	js["WARN_annunIRS"] = data.WARN_annunIRS != 0;
	js["WARN_annunFUEL"] = data.WARN_annunFUEL != 0;
	js["WARN_annunELEC"] = data.WARN_annunELEC != 0;
	js["WARN_annunAPU"] = data.WARN_annunAPU != 0;
	js["WARN_annunOVHT_DET"] = data.WARN_annunOVHT_DET != 0;
	js["WARN_annunANTI_ICE"] = data.WARN_annunANTI_ICE != 0;
	js["WARN_annunHYD"] = data.WARN_annunHYD != 0;
	js["WARN_annunDOORS"] = data.WARN_annunDOORS != 0;
	js["WARN_annunENG"] = data.WARN_annunENG != 0;
	js["WARN_annunOVERHEAD"] = data.WARN_annunOVERHEAD != 0;
	js["WARN_annunAIR_COND"] = data.WARN_annunAIR_COND != 0;
	js["MAIN_annunSPEEDBRAKE_ARMED"] = data.MAIN_annunSPEEDBRAKE_ARMED != 0;
	js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM != 0;
	js["MAIN_annunSPEEDBRAKE_EXTENDED"] = data.MAIN_annunSPEEDBRAKE_EXTENDED != 0;
	js["MAIN_annunSTAB_OUT_OF_TRIM"] = data.MAIN_annunSTAB_OUT_OF_TRIM != 0;
	js["MAIN_annunANTI_SKID_INOP"] = data.MAIN_annunANTI_SKID_INOP != 0;
	js["MAIN_annunAUTO_BRAKE_DISARM"] = data.MAIN_annunAUTO_BRAKE_DISARM != 0;
	js["MAIN_annunLE_FLAPS_TRANSIT"] = data.MAIN_annunLE_FLAPS_TRANSIT != 0;
	js["MAIN_annunLE_FLAPS_EXT"] = data.MAIN_annunLE_FLAPS_EXT != 0;
	js["HGS_annun_AIII"] = data.HGS_annun_AIII != 0;
	js["HGS_annun_NO_AIII"] = data.HGS_annun_NO_AIII != 0;
	js["HGS_annun_FLARE"] = data.HGS_annun_FLARE != 0;
	js["HGS_annun_RO"] = data.HGS_annun_RO != 0;
	js["HGS_annun_RO_CTN"] = data.HGS_annun_RO_CTN != 0;
	js["HGS_annun_RO_ARM"] = data.HGS_annun_RO_ARM != 0;
	js["HGS_annun_TO"] = data.HGS_annun_TO != 0;
	js["HGS_annun_TO_CTN"] = data.HGS_annun_TO_CTN != 0;
	js["HGS_annun_APCH"] = data.HGS_annun_APCH != 0;
	js["HGS_annun_TO_WARN"] = data.HGS_annun_TO_WARN != 0;
	js["HGS_annun_Bar"] = data.HGS_annun_Bar != 0;
	js["HGS_annun_FAIL"] = data.HGS_annun_FAIL != 0;
	js["GPWS_annunINOP"] = data.GPWS_annunINOP != 0;
	js["PED_annunParkingBrake"] = data.PED_annunParkingBrake != 0;
	js["FIRE_annunWHEEL_WELL"] = data.FIRE_annunWHEEL_WELL != 0;
	js["FIRE_annunFAULT"] = data.FIRE_annunFAULT != 0;
	js["FIRE_annunAPU_DET_INOP"] = data.FIRE_annunAPU_DET_INOP != 0;
	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = data.FIRE_annunAPU_BOTTLE_DISCHARGE != 0;
	js["CARGO_annunFWD"] = data.CARGO_annunFWD != 0;
	js["CARGO_annunAFT"] = data.CARGO_annunAFT != 0;
	js["CARGO_annunDETECTOR_FAULT"] = data.CARGO_annunDETECTOR_FAULT != 0;
	js["CARGO_annunDISCH"] = data.CARGO_annunDISCH != 0;
	js["HGS_annunRWY"] = data.HGS_annunRWY != 0;
	js["HGS_annunGS"] = data.HGS_annunGS != 0;
	js["HGS_annunFAULT"] = data.HGS_annunFAULT != 0;
	js["HGS_annunCLR"] = data.HGS_annunCLR != 0;
	js["XPDR_annunFAIL"] = data.XPDR_annunFAIL != 0;
	js["PED_annunLOCK_FAIL"] = data.PED_annunLOCK_FAIL != 0;
	js["PED_annunAUTO_UNLK"] = data.PED_annunAUTO_UNLK != 0;
	js["DOOR_annunFWD_ENTRY"] = data.DOOR_annunFWD_ENTRY != 0;
	js["DOOR_annunFWD_SERVICE"] = data.DOOR_annunFWD_SERVICE != 0;
	js["DOOR_annunAIRSTAIR"] = data.DOOR_annunAIRSTAIR != 0;
	js["DOOR_annunLEFT_FWD_OVERWING"] = data.DOOR_annunLEFT_FWD_OVERWING != 0;
	js["DOOR_annunRIGHT_FWD_OVERWING"] = data.DOOR_annunRIGHT_FWD_OVERWING != 0;
	js["DOOR_annunFWD_CARGO"] = data.DOOR_annunFWD_CARGO != 0;
	js["DOOR_annunEQUIP"] = data.DOOR_annunEQUIP != 0;
	js["DOOR_annunLEFT_AFT_OVERWING"] = data.DOOR_annunLEFT_AFT_OVERWING != 0;
	js["DOOR_annunRIGHT_AFT_OVERWING"] = data.DOOR_annunRIGHT_AFT_OVERWING != 0;
	js["DOOR_annunAFT_CARGO"] = data.DOOR_annunAFT_CARGO != 0;
	js["DOOR_annunAFT_ENTRY"] = data.DOOR_annunAFT_ENTRY != 0;
	js["DOOR_annunAFT_SERVICE"] = data.DOOR_annunAFT_SERVICE != 0;
	js["AIR_annunAUTO_FAIL"] = data.AIR_annunAUTO_FAIL != 0;
	js["AIR_annunOFFSCHED_DESCENT"] = data.AIR_annunOFFSCHED_DESCENT != 0;
	js["AIR_annunALTN"] = data.AIR_annunALTN != 0;
	js["AIR_annunMANUAL"] = data.AIR_annunMANUAL != 0;
	js["IRS_annunALIGN_1"] = data.IRS_annunALIGN[0] != 0;
	js["IRS_annunALIGN_2"] = data.IRS_annunALIGN[1] != 0;
	js["IRS_annunON_DC_1"] = data.IRS_annunON_DC[0] != 0;
	js["IRS_annunON_DC_2"] = data.IRS_annunON_DC[1] != 0;
	js["IRS_annunFAULT_1"] = data.IRS_annunFAULT[0] != 0;
	js["IRS_annunFAULT_2"] = data.IRS_annunFAULT[1] != 0;
	js["IRS_annunDC_FAIL_1"] = data.IRS_annunDC_FAIL[0] != 0;
	js["IRS_annunDC_FAIL_2"] = data.IRS_annunDC_FAIL[1] != 0;
	js["ENG_annunREVERSER_1"] = data.ENG_annunREVERSER[0] != 0;
	js["ENG_annunREVERSER_2"] = data.ENG_annunREVERSER[1] != 0;
	js["ENG_annunENGINE_CONTROL_1"] = data.ENG_annunENGINE_CONTROL[0] != 0;
	js["ENG_annunENGINE_CONTROL_2"] = data.ENG_annunENGINE_CONTROL[1] != 0;
	js["ENG_annunALTN_1"] = data.ENG_annunALTN[0] != 0;
	js["ENG_annunALTN_2"] = data.ENG_annunALTN[1] != 0;
	js["FCTL_annunFC_LOW_PRESSURE_1"] = data.FCTL_annunFC_LOW_PRESSURE[0] != 0;
	js["FCTL_annunFC_LOW_PRESSURE_2"] = data.FCTL_annunFC_LOW_PRESSURE[1] != 0;

	js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0] != 0 != 0;
	js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1] != 0 != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0] != 0 != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1] != 0 != 0;

	js["FUEL_annunFILTER_BYPASS_1"] = data.FUEL_annunFILTER_BYPASS[0] != 0;
	js["FUEL_annunFILTER_BYPASS_2"] = data.FUEL_annunFILTER_BYPASS[1] != 0;
	js["FUEL_annunLOWPRESS_Fwd_1"] = data.FUEL_annunLOWPRESS_Fwd[0] != 0;
	js["FUEL_annunLOWPRESS_Fwd_2"] = data.FUEL_annunLOWPRESS_Fwd[1] != 0;
	js["FUEL_annunLOWPRESS_Aft_1"] = data.FUEL_annunLOWPRESS_Aft[0] != 0;
	js["FUEL_annunLOWPRESS_Aft_2"] = data.FUEL_annunLOWPRESS_Aft[1] != 0;
	js["FUEL_annunLOWPRESS_Ctr_1"] = data.FUEL_annunLOWPRESS_Ctr[0] != 0;
	js["FUEL_annunLOWPRESS_Ctr_2"] = data.FUEL_annunLOWPRESS_Ctr[1] != 0;
	js["ELEC_annunDRIVE_1"] = data.ELEC_annunDRIVE[0] != 0;
	js["ELEC_annunDRIVE_2"] = data.ELEC_annunDRIVE[1] != 0;
	js["ELEC_annunTRANSFER_BUS_OFF_1"] = data.ELEC_annunTRANSFER_BUS_OFF[0] != 0;
	js["ELEC_annunTRANSFER_BUS_OFF_2"] = data.ELEC_annunTRANSFER_BUS_OFF[1] != 0;
	js["ELEC_annunSOURCE_OFF_1"] = data.ELEC_annunSOURCE_OFF[0] != 0;
	js["ELEC_annunSOURCE_OFF_2"] = data.ELEC_annunSOURCE_OFF[1] != 0;
	js["ELEC_annunGEN_BUS_OFF_1"] = data.ELEC_annunGEN_BUS_OFF[0] != 0;
	js["ELEC_annunGEN_BUS_OFF_2"] = data.ELEC_annunGEN_BUS_OFF[1] != 0;
	js["ICE_annunOVERHEAT_1"] = data.ICE_annunOVERHEAT[0] != 0;
	js["ICE_annunOVERHEAT_2"] = data.ICE_annunOVERHEAT[1] != 0;
	js["ICE_annunOVERHEAT_3"] = data.ICE_annunOVERHEAT[2] != 0;
	js["ICE_annunOVERHEAT_4"] = data.ICE_annunOVERHEAT[3] != 0;
	js["ICE_annunON_1"] = data.ICE_annunON[0] != 0;
	js["ICE_annunON_2"] = data.ICE_annunON[1] != 0;
	js["ICE_annunON_3"] = data.ICE_annunON[2] != 0;
	js["ICE_annunON_4"] = data.ICE_annunON[3] != 0;
	js["ICE_annunVALVE_OPEN_1"] = data.ICE_annunVALVE_OPEN[0] != 0;
	js["ICE_annunVALVE_OPEN_2"] = data.ICE_annunVALVE_OPEN[1] != 0;
	js["ICE_annunCOWL_ANTI_ICE_1"] = data.ICE_annunCOWL_ANTI_ICE[0] != 0;
	js["ICE_annunCOWL_ANTI_ICE_2"] = data.ICE_annunCOWL_ANTI_ICE[1] != 0;
	js["ICE_annunCOWL_VALVE_OPEN_1"] = data.ICE_annunCOWL_VALVE_OPEN[0] != 0;
	js["ICE_annunCOWL_VALVE_OPEN_2"] = data.ICE_annunCOWL_VALVE_OPEN[1] != 0;
	js["HYD_annunLOW_PRESS_eng_1"] = data.HYD_annunLOW_PRESS_eng[0] != 0;
	js["HYD_annunLOW_PRESS_eng_2"] = data.HYD_annunLOW_PRESS_eng[1] != 0;
	js["HYD_annunLOW_PRESS_elec_1"] = data.HYD_annunLOW_PRESS_elec[0] != 0;
	js["HYD_annunLOW_PRESS_elec_2"] = data.HYD_annunLOW_PRESS_elec[1] != 0;
	js["HYD_annunOVERHEAT_elec_1"] = data.HYD_annunOVERHEAT_elec[0] != 0;
	js["HYD_annunOVERHEAT_elec_2"] = data.HYD_annunOVERHEAT_elec[1] != 0;
	js["AIR_annunZoneTemp_1"] = data.AIR_annunZoneTemp[0] != 0;
	js["AIR_annunZoneTemp_2"] = data.AIR_annunZoneTemp[1] != 0;
	js["AIR_annunZoneTemp_3"] = data.AIR_annunZoneTemp[2] != 0;
	js["AIR_annunPackTripOff_1"] = data.AIR_annunPackTripOff[0] != 0;
	js["AIR_annunPackTripOff_2"] = data.AIR_annunPackTripOff[1] != 0;
	js["AIR_annunWingBodyOverheat_1"] = data.AIR_annunWingBodyOverheat[0] != 0;
	js["AIR_annunWingBodyOverheat_2"] = data.AIR_annunWingBodyOverheat[1] != 0;
	js["AIR_annunBleedTripOff_1"] = data.AIR_annunBleedTripOff[0] != 0;
	js["AIR_annunBleedTripOff_2"] = data.AIR_annunBleedTripOff[1] != 0;
	js["WARN_annunFIRE_WARN_1"] = data.WARN_annunFIRE_WARN[0] != 0;
	js["WARN_annunFIRE_WARN_2"] = data.WARN_annunFIRE_WARN[1] != 0;
	js["WARN_annunMASTER_CAUTION_1"] = data.WARN_annunMASTER_CAUTION[0] != 0;
	js["WARN_annunMASTER_CAUTION_2"] = data.WARN_annunMASTER_CAUTION[1] != 0;
	js["MAIN_annunBELOW_GS_1"] = data.MAIN_annunBELOW_GS[0] != 0;
	js["MAIN_annunBELOW_GS_2"] = data.MAIN_annunBELOW_GS[1] != 0;
	js["MAIN_annunAP_1"] = data.MAIN_annunAP[0] != 0;
	js["MAIN_annunAP_2"] = data.MAIN_annunAP[1] != 0;
	js["MAIN_annunAT_1"] = data.MAIN_annunAT[0] != 0;
	js["MAIN_annunAT_2"] = data.MAIN_annunAT[1] != 0;
	js["MAIN_annunFMC_1"] = data.MAIN_annunFMC[0] != 0;
	js["MAIN_annunFMC_2"] = data.MAIN_annunFMC[1] != 0;
	js["MAIN_annunGEAR_transit_1"] = data.MAIN_annunGEAR_transit[0] != 0;
	js["MAIN_annunGEAR_transit_2"] = data.MAIN_annunGEAR_transit[1] != 0;
	js["MAIN_annunGEAR_transit_3"] = data.MAIN_annunGEAR_transit[2] != 0;
	js["MAIN_annunGEAR_locked_1"] = data.MAIN_annunGEAR_locked[0] != 0;
	js["MAIN_annunGEAR_locked_2"] = data.MAIN_annunGEAR_locked[1] != 0;
	js["MAIN_annunGEAR_locked_3"] = data.MAIN_annunGEAR_locked[2] != 0;
	js["FIRE_annunENG_OVERHEAT_1"] = data.FIRE_annunENG_OVERHEAT[0] != 0;
	js["FIRE_annunENG_OVERHEAT_2"] = data.FIRE_annunENG_OVERHEAT[1] != 0;
	js["FIRE_annunBOTTLE_DISCHARGE_1"] = data.FIRE_annunBOTTLE_DISCHARGE[0] != 0;
	js["FIRE_annunBOTTLE_DISCHARGE_2"] = data.FIRE_annunBOTTLE_DISCHARGE[1] != 0;
	js["FIRE_annunExtinguisherTest_1"] = data.FIRE_annunExtinguisherTest[0] != 0;
	js["FIRE_annunExtinguisherTest_2"] = data.FIRE_annunExtinguisherTest[1] != 0;
	js["FIRE_annunExtinguisherTest_3"] = data.FIRE_annunExtinguisherTest[2] != 0;
	js["CARGO_annunExtTest_1"] = data.CARGO_annunExtTest[0] != 0;
	js["CARGO_annunExtTest_2"] = data.CARGO_annunExtTest[1] != 0;
	js["MAIN_annunAP_Amber_1"] = data.MAIN_annunAP_Amber[0] != 0;
	js["MAIN_annunAP_Amber_2"] = data.MAIN_annunAP_Amber[1] != 0;
	js["MAIN_annunAT_Amber_1"] = data.MAIN_annunAT_Amber[0] != 0;
	js["MAIN_annunAT_Amber_2"] = data.MAIN_annunAT_Amber[1] != 0;
	js["MCP_FDSw_1"] = data.FD_left_Switches_Status != 0;
	js["MCP_FDSw_2"] = data.FD_right_Switches_Status != 0;
	js["EFIS_ModeSel_1"] = data.FO_ND_Mode_status;
	js["EFIS_ModeSel_2"] = data.FO_ND_Mode_status;
	js["MCP_DisengageBar"] = shareMemSDK->DISENGAGE_Bar_Switches_Status != 0;

	return js.dump();
}



std::string Ifly737::buildJsonIfly737()
{
	ShareMemorySDK data;
	json js;

	if (ngxData.DISENGAGE_Bar_Switches_Status != shareMemSDK->DISENGAGE_Bar_Switches_Status) {
		ngxData.DISENGAGE_Bar_Switches_Status = shareMemSDK->DISENGAGE_Bar_Switches_Status;
		js["MCP_DisengageBar"] = shareMemSDK->DISENGAGE_Bar_Switches_Status != 0;
	}
	if (ngxData.FO_ND_Mode_status != data.FO_ND_Mode_status)
	{
		ngxData.FO_ND_Mode_status = data.FO_ND_Mode_status;
		js["EFIS_ModeSel_1"] = data.FO_ND_Mode_status;
	}
	if (ngxData.FO_ND_Mode_status != data.FO_ND_Mode_status)
	{
		ngxData.FO_ND_Mode_status = data.FO_ND_Mode_status;
		js["EFIS_ModeSel_2"] = data.FO_ND_Mode_status != 0;
	}
	if (ngxData.FD_left_Switches_Status != data.FD_left_Switches_Status) {
		ngxData.FD_left_Switches_Status = data.FD_left_Switches_Status;
		js["MCP_FDSw_1"] = data.FD_left_Switches_Status != 0;
	}
	if (ngxData.FD_right_Switches_Status != data.FD_right_Switches_Status) {
		ngxData.FD_right_Switches_Status = data.FD_right_Switches_Status;
		js["MCP_FDSw_2"] = data.FD_right_Switches_Status != 0;
	}

	if (abs(ngxData.Oxygen_Pressure - data.Oxygen_Pressure) > 20)
	{
		ngxData.Oxygen_Pressure = data.Oxygen_Pressure;
		js["OXY_Needle"] = data.Oxygen_Pressure != 0;
	}
	//if (ngxData.IRS_DisplaySelector != data.IRS_DisplaySelector) {
	//	ngxData.IRS_DisplaySelector = data.IRS_DisplaySelector;
	//	js["IRS_DisplaySelector"] = data.IRS_DisplaySelector;
	//}

	//if (ngxData.IRS_SysDisplay_R != data.IRS_SysDisplay_R) {
	//	ngxData.IRS_SysDisplay_R = data.IRS_SysDisplay_R;
	//	js["IRS_SysDisplay_R"] = data.IRS_SysDisplay_R;
	//}

	//if (ngxData.COMM_ServiceInterphoneSw != data.COMM_ServiceInterphoneSw) {
	//	ngxData.COMM_ServiceInterphoneSw = data.COMM_ServiceInterphoneSw;
	//	js["COMM_ServiceInterphoneSw"] = data.COMM_ServiceInterphoneSw != 0;
	//}
	if (ngxData.Oxygen_Switch_Status != data.Oxygen_Switch_Status) {
		ngxData.Oxygen_Switch_Status = data.Oxygen_Switch_Status;
		js["OXY_SwNormal"] = data.Oxygen_Switch_Status != 0;
	}
	if (ngxData.FLTREC_SwNormal != data.FLTREC_SwNormal) {
		ngxData.FLTREC_SwNormal = data.FLTREC_SwNormal;
		js["FLTREC_SwNormal"] = !data.FLTREC_SwNormal != 0;

	}
	if (ngxData.Yaw_Damper_Switches_Status != data.Yaw_Damper_Switches_Status) {
		ngxData.Yaw_Damper_Switches_Status = data.Yaw_Damper_Switches_Status;
		js["FCTL_YawDamper_Sw"] = data.Yaw_Damper_Switches_Status != 0;
	}
	if (ngxData.Altn_Flap_Switches_Status != data.Altn_Flap_Switches_Status) {
		ngxData.Altn_Flap_Switches_Status = data.Altn_Flap_Switches_Status;
		js["FCTL_AltnFlaps_Control_Sw"] = data.Altn_Flap_Switches_Status;
	}
	if (ngxData.Altn_Flap_Master_Switches_Status != data.Altn_Flap_Master_Switches_Status) {
		ngxData.Altn_Flap_Master_Switches_Status = data.Altn_Flap_Master_Switches_Status;
		js["FCTL_AltnFlaps_Sw_ARM"]  = data.Altn_Flap_Master_Switches_Status != 0;
	}
	if (ngxData.VHF_NAV_Switches_Status != data.VHF_NAV_Switches_Status) {
		ngxData.VHF_NAV_Switches_Status = data.VHF_NAV_Switches_Status;
		js["NAVDIS_VHFNavSelector"] = data.VHF_NAV_Switches_Status;
	}
	if (ngxData.IRS_Switches_Status != data.IRS_Switches_Status) {
		ngxData.IRS_Switches_Status = data.IRS_Switches_Status;
		js["NAVDIS_IRSSelector"] = data.IRS_Switches_Status;
	}
	if (ngxData.FMC_Switches_Status != data.FMC_Switches_Status) {
		ngxData.FMC_Switches_Status = data.FMC_Switches_Status;
		js["NAVDIS_FMCSelector"] = data.FMC_Switches_Status;
	}
	if (ngxData..Source_Switches_Status != data..Source_Switches_Status) {
		ngxData..Source_Switches_Status = data..Source_Switches_Status;
		js["NAVDIS_SourceSelector"] = data..Source_Switches_Status;
	}
	if (ngxData.Control_Panel_Switches_Status != data.Control_Panel_Switches_Status) {
		ngxData.Control_Panel_Switches_Status = data.Control_Panel_Switches_Status;
		js["NAVDIS_ControlPaneSelector"] = data.Control_Panel_Switches_Status;
	}
	if (ngxData.Fuel_Crossfeed_Selector_Status != data.Fuel_Crossfeed_Selector_Status) {
		ngxData.Fuel_Crossfeed_Selector_Status = data.Fuel_Crossfeed_Selector_Status;
		js["FUEL_CrossFeedSw"] = data.Fuel_Crossfeed_Selector_Status != 0;
	}
	if (ngxData.DC_Meters_Selector_Status != data.DC_Meters_Selector_Status) {
		ngxData.DC_Meters_Selector_Status = data.DC_Meters_Selector_Status;
		js["ELEC_DCMeterSelector"] = data.DC_Meters_Selector_Status;
	}
	//if (ngxData.AC_Meters_Selector_Status != data.AC_Meters_Selector_Status) {
	//	ngxData.AC_Meters_Selector_Status = data.AC_Meters_Selector_Status;
	//	js["ELEC_ACMeterSelector"] = data.AC_Meters_Selector_Status;
	//}
	if (ngxData.Battery_Switch_Status != data.Battery_Switch_Status) {
		ngxData.Battery_Switch_Status = data.Battery_Switch_Status;
		js["ELEC_BatSelector"] = data.Battery_Switch_Status != 0;
	}
	if (ngxData.CAB_UTIL_Switch_Status != data.CAB_UTIL_Switch_Status) {
		ngxData.CAB_UTIL_Switch_Status = data.CAB_UTIL_Switch_Status;
		js["ELEC_CabUtilSw"] = data.CAB_UTIL_Switch_Status != 0;
	}
	if (ngxData.IFE_PASS_SEAT_Switches_Status != data.IFE_PASS_SEAT_Switches_Status) {
		ngxData.IFE_PASS_SEAT_Switches_Status = data.IFE_PASS_SEAT_Switches_Status;
		js["ELEC_IFEPassSeatSw"] = data.IFE_PASS_SEAT_Switches_Status != 0;
	}
	if (ngxData.STANDBY_POWER_Switch_Status != data.STANDBY_POWER_Switch_Status) {
		ngxData.STANDBY_POWER_Switch_Status = data.STANDBY_POWER_Switch_Status;
		js["ELEC_StandbyPowerSelector"] = data.STANDBY_POWER_Switch_Status;
	}
	if (ngxData.Ground_Power_Switches_Status != data.Ground_Power_Switches_Status) {
		ngxData.Ground_Power_Switches_Status = data.Ground_Power_Switches_Status;
		js["ELEC_GrdPwrSw"] = data.Ground_Power_Switches_Status != 0;
	}
	if (ngxData.Bus_Transfer_Switches_Status != data.Bus_Transfer_Switches_Status) {
		ngxData.Bus_Transfer_Switches_Status = data.Bus_Transfer_Switches_Status;
		js["ELEC_BusTransSw_AUTO"] = data.Bus_Transfer_Switches_Status != 0;
	}
	if (ngxData.Wiper_L_Switches_Status != data.Wiper_L_Switches_Status) {
		ngxData.Wiper_L_Switches_Status = data.Wiper_L_Switches_Status;
		js["OH_WiperLSelector"] = data.Wiper_L_Switches_Status;
	}
	if (ngxData.Wiper_R_Switches_Status != data.Wiper_R_Switches_Status) {
		ngxData.Wiper_R_Switches_Status = data.Wiper_R_Switches_Status;
		js["OH_WiperRSelector"] = data.Wiper_R_Switches_Status;
	}
	//if (ngxData.LTS_CircuitBreakerKnob != data.LTS_CircuitBreakerKnob) {
	//	ngxData.LTS_CircuitBreakerKnob = data.LTS_CircuitBreakerKnob;
	//	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	//}
	//if (ngxData.LTS_CircuitBreakerKnob != data.LTS_CircuitBreakerKnob) {
	//	ngxData.LTS_CircuitBreakerKnob = data.LTS_CircuitBreakerKnob;
	//	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	//}
	//if (ngxData.LTS_OvereadPanelKnob != data.LTS_OvereadPanelKnob) {
	//	ngxData.LTS_OvereadPanelKnob = data.LTS_OvereadPanelKnob;
	//	js["LTS_OvereadPanelKnob"] = data.LTS_OvereadPanelKnob;
	//}
	if (ngxData.Equipment_COOLING_SUPPLY_Switch_Status != data.Equipment_COOLING_SUPPLY_Switch_Status) {
		ngxData.Equipment_COOLING_SUPPLY_Switch_Status = data.Equipment_COOLING_SUPPLY_Switch_Status;
		js["AIR_EquipCoolingSupplyNORM"] = data.Equipment_COOLING_SUPPLY_Switch_Status != 0;
	}
	if (ngxData.Equipment_COOLING_EXHAUST_Switch_Status != data.Equipment_COOLING_EXHAUST_Switch_Status) {
		ngxData.Equipment_COOLING_EXHAUST_Switch_Status = data.Equipment_COOLING_EXHAUST_Switch_Status;
		js["AIR_EquipCoolingExhaustNORM"] = data.Equipment_COOLING_EXHAUST_Switch_Status != 0;
	}
	if (ngxData.Emergency_Light_Switch_Status != data.Emergency_Light_Switch_Status) {
		ngxData.Emergency_Light_Switch_Status = data.Emergency_Light_Switch_Status;
		js["LTS_EmerExitSelector"] = data.Emergency_Light_Switch_Status;
	}
	//if (ngxData.COMM_NoSmokingSelector != data.COMM_NoSmokingSelector) {
	//	ngxData.COMM_NoSmokingSelector = data.COMM_NoSmokingSelector;
	//	js["COMM_NoSmokingSelector"] = data.COMM_NoSmokingSelector != 0;
	//}
	if (ngxData.Fasten_Belts_Switches_Status != data.Fasten_Belts_Switches_Status) {
		ngxData.Fasten_Belts_Switches_Status = data.Fasten_Belts_Switches_Status;
		js["COMM_FastenBeltsSelector"] = data.Fasten_Belts_Switches_Status != 0;
	}
	if (ngxData.Wing_AntiIce_Switch_Status != data.Wing_AntiIce_Switch_Status) {
		ngxData.Wing_AntiIce_Switch_Status = data.Wing_AntiIce_Switch_Status;
		js["ICE_WingAntiIceSw"] = data.Wing_AntiIce_Switch_Status != 0;
	}
	//if (ngxData.AIR_TempSourceSelector != data.AIR_TempSourceSelector) {
	//	ngxData.AIR_TempSourceSelector = data.AIR_TempSourceSelector;
	//	js["AIR_TempSourceSelector"] = data.AIR_TempSourceSelector != 0;
	//}
	if (ngxData.TRIM_AIR_Switch_Status != data.TRIM_AIR_Switch_Status) {
		ngxData.TRIM_AIR_Switch_Status = data.TRIM_AIR_Switch_Status;
		js["AIR_TrimAirSwitch"] = data.TRIM_AIR_Switch_Status != 0;
	}
	if (ngxData.APU_Bleed_Air_Switches_Status != data.APU_Bleed_Air_Switches_Status) {
		ngxData.APU_Bleed_Air_Switches_Status = data.APU_Bleed_Air_Switches_Status;
		js["AIR_APUBleedAirSwitch"] = data.APU_Bleed_Air_Switches_Status != 0;
	}
	if (ngxData.Isolation_Valve_Switches_Status != data.Isolation_Valve_Switches_Status) {
		ngxData.Isolation_Valve_Switches_Status = data.Isolation_Valve_Switches_Status;
		js["AIR_IsolationValveSwitch"] = data.Isolation_Valve_Switches_Status;
	}
	//if (ngxData.AIR_OutflowValveSwitch != data.AIR_OutflowValveSwitch) {
	//	ngxData.AIR_OutflowValveSwitch = data.AIR_OutflowValveSwitch;
	//	js["AIR_OutflowValveSwitch"] = data.AIR_OutflowValveSwitch != 0;
	//}
	if (ngxData.Pressurization_Mode_Selector_Status != data.Pressurization_Mode_Selector_Status) {
		ngxData.Pressurization_Mode_Selector_Status = data.Pressurization_Mode_Selector_Status;
		js["AIR_PressurizationModeSelector"] = data.Pressurization_Mode_Selector_Status;
	}
	if (ngxData.Taxi_Light_Status != data.Taxi_Light_Status) {
		ngxData.Taxi_Light_Status = data.Taxi_Light_Status;
		js["LTS_TaxiSw"] = data.Taxi_Light_Status != 0;
	}
	if (ngxData.APU_Switches_Status != data.APU_Switches_Status) {
		ngxData.APU_Switches_Status = data.APU_Switches_Status;
		js["APU_Selector"] = data.APU_Switches_Status;
	}
	if (ngxData.Ignition_Select_Switches_Status != data.Ignition_Select_Switches_Status) {
		ngxData.Ignition_Select_Switches_Status = data.Ignition_Select_Switches_Status;
		js["ENG_IgnitionSelector"] = data.Ignition_Select_Switches_Status;
	}
	//if (ngxData.LTS_LogoSw != data.LTS_LogoSw) {
	//	ngxData.LTS_LogoSw = data.LTS_LogoSw;
	//	js["LTS_LogoSw"] = data.LTS_LogoSw != 0;
	//}
	if (ngxData.Position_Light_Status != data.Position_Light_Status) {
		ngxData.Position_Light_Status = data.Position_Light_Status;
		js["LTS_PositionSw"] = data.Position_Light_Status != 0;
	}
	if (ngxData.Anti_Collision_Light_Status != data.Anti_Collision_Light_Status) {
		ngxData.Anti_Collision_Light_Status = data.Anti_Collision_Light_Status;
		js["LTS_AntiCollisionSw"] = data.Anti_Collision_Light_Status;
	}
	if (ngxData.Wing_Light_Status != data.Wing_Light_Status) {
		ngxData.Wing_Light_Status = data.Wing_Light_Status;
		js["LTS_WingSw"] = data.Wing_Light_Status != 0;
	}
	if (ngxData.WHEEL_WELL_Light_Status != data.WHEEL_WELL_Light_Status) {
		ngxData.WHEEL_WELL_Light_Status = data.WHEEL_WELL_Light_Status;
		js["LTS_WheelWellSw"] = data.WHEEL_WELL_Light_Status != 0;
	}
	if (ngxData.MAIN_NoseWheelSteeringSwNORM != data.MAIN_NoseWheelSteeringSwNORM) {
		ngxData.MAIN_NoseWheelSteeringSwNORM = data.MAIN_NoseWheelSteeringSwNORM;
		js["MAIN_NoseWheelSteeringSwNORM"] = data.MAIN_NoseWheelSteeringSwNORM != 0;
	}
	//if (ngxData.MAIN_LightsSelector != data.MAIN_LightsSelector) {
	//	ngxData.MAIN_LightsSelector = data.MAIN_LightsSelector;
	//	js["MAIN_LightsSelector"] = data.MAIN_LightsSelector != 0;
	//}
	//if (ngxData.MAIN_RMISelector1_VOR != data.MAIN_RMISelector1_VOR) {
	//	ngxData.MAIN_RMISelector1_VOR = data.MAIN_RMISelector1_VOR;
	//	js["MAIN_RMISelector1_VOR"] = data.MAIN_RMISelector1_VOR != 0;
	//}
	//if (ngxData.MAIN_RMISelector2_VOR != data.MAIN_RMISelector2_VOR) {
	//	ngxData.MAIN_RMISelector2_VOR = data.MAIN_RMISelector2_VOR;
	//	js["MAIN_RMISelector2_VOR"] = data.MAIN_RMISelector2_VOR != 0;
	//}
	if (ngxData.N1_Set_Status != data.N1_Set_Status) {
		ngxData.N1_Set_Status = data.N1_Set_Status;
		js["MAIN_N1SetSelector"] = data.N1_Set_Status;
	}
	if (ngxData.N1_Set_Status != data.N1_Set_Status) {
		ngxData.N1_Set_Status = data.N1_Set_Status;
		js["MAIN_SpdRefSelector"] = data.N1_Set_Status;
	}
	if (ngxData.Fuel_Flow_Status != data.Fuel_Flow_Status) {
		ngxData.Fuel_Flow_Status = data.Fuel_Flow_Status;
		js["MAIN_FuelFlowSelector"] = data.Fuel_Flow_Status;
	}
	if (ngxData.Autobrake_Switches_Status != data.Autobrake_Switches_Status) {
		ngxData.Autobrake_Switches_Status = data.Autobrake_Switches_Status;
		js["MAIN_AutobrakeSelector"] = data.Autobrake_Switches_Status;
	}
	if (ngxData.Gear_Lever_Status != data.Gear_Lever_Status) {
		ngxData.Gear_Lever_Status = data.Gear_Lever_Status;
		js["MAIN_GearLever"] = data.Gear_Lever_Status;
	}
	if (ngxData.Flap_Inhibit_Switches_Status != data.Flap_Inhibit_Switches_Status) {
		ngxData.Flap_Inhibit_Switches_Status = data.Flap_Inhibit_Switches_Status;
		js["GPWS_FlapInhibitSw_NORM"] = data.Flap_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Gear_Inhibit_Switches_Status != data.Gear_Inhibit_Switches_Status) {
		ngxData.Gear_Inhibit_Switches_Status = data.Gear_Inhibit_Switches_Status;
		js["GPWS_GearInhibitSw_NORM"] = data.Gear_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Terr_Inhibit_Switches_Status != data.Terr_Inhibit_Switches_Status) {
		ngxData.Terr_Inhibit_Switches_Status = data.Terr_Inhibit_Switches_Status;
		js["GPWS_TerrInhibitSw_NORM"] = data.Terr_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Stab_Trim_Main_Elect_Switch_Status != data.Stab_Trim_Main_Elect_Switch_Status) {
		ngxData.Stab_Trim_Main_Elect_Switch_Status = data.Stab_Trim_Main_Elect_Switch_Status;
		js["TRIM_StabTrimMainElecSw_NORMAL"] = data.Stab_Trim_Main_Elect_Switch_Status != 0;
	}
	if (ngxData.Stab_Trim_Auto_Pilot_Switch_Status != data.Stab_Trim_Auto_Pilot_Switch_Status) {
		ngxData.Stab_Trim_Auto_Pilot_Switch_Status = data.Stab_Trim_Auto_Pilot_Switch_Status;
		js["TRIM_StabTrimAutoPilotSw_NORMAL"] = data.Stab_Trim_Auto_Pilot_Switch_Status != 0;
	}
	//if (ngxData.FIRE_DetTestSw != data.FIRE_DetTestSw) {
	//	ngxData.FIRE_DetTestSw = data.FIRE_DetTestSw;
	//	js["FIRE_DetTestSw"] = data.FIRE_DetTestSw != 0;
	//}
	//if (ngxData.FIRE_ExtinguisherTestSw != data.FIRE_ExtinguisherTestSw) {
	//	ngxData.FIRE_ExtinguisherTestSw = data.FIRE_ExtinguisherTestSw;
	//	js["FIRE_ExtinguisherTestSw"] = data.FIRE_ExtinguisherTestSw != 0;
	//}
	//if (ngxData.XPDR_XpndrSelector_2 != data.XPDR_XpndrSelector_2) {
	//	ngxData.XPDR_XpndrSelector_2 = data.XPDR_XpndrSelector_2;
	//	js["XPDR_XpndrSelector_2"] = data.XPDR_XpndrSelector_2 != 0;
	//}
	//if (ngxData.XPDR_AltSourceSel_2 != data.XPDR_AltSourceSel_2) {
	//	ngxData.XPDR_AltSourceSel_2 = data.XPDR_AltSourceSel_2;
	//	js["XPDR_AltSourceSel_2"] = data.XPDR_AltSourceSel_2 != 0;
	//}
	//if (ngxData.XPDR_ModeSel != data.XPDR_ModeSel) {
	//	ngxData.XPDR_ModeSel = data.XPDR_ModeSel;
	//	js["XPDR_ModeSel"] = data.XPDR_ModeSel != 0;
	//}
	if (ngxData.Stabilizer_Trim_Switch_Status != data.Stabilizer_Trim_Switch_Status) {
		ngxData.Stabilizer_Trim_Switch_Status = data.Stabilizer_Trim_Switch_Status;
		js["TRIM_StabTrimSw_NORMAL"] = data.Stabilizer_Trim_Switch_Status != 0;
	}
	//if (ngxData.PED_FltDkDoorSel != data.PED_FltDkDoorSel) {
	//	ngxData.PED_FltDkDoorSel = data.PED_FltDkDoorSel;
	//	js["PED_FltDkDoorSel"] = data.PED_FltDkDoorSel != 0;
	//}
	if (ngxData.IRS_aligned != data.IRS_aligned) {
		ngxData.IRS_aligned = data.IRS_aligned;
		js["IRS_aligned"] = data.IRS_aligned != 0;
	}
	//if (ngxData.AircraftModel != data.AircraftModel) {
	//	ngxData.AircraftModel = data.AircraftModel;
	//	js["AircraftModel"] = data.AircraftModel != 0;
	//}
	//if (ngxData.WeightInKg != data.WeightInKg) {
	//	ngxData.WeightInKg = data.WeightInKg;
	//	js["WeightInKg"] = data.WeightInKg != 0;
	//}
	//if (ngxData.GPWS_V1CallEnabled != data.GPWS_V1CallEnabled) {
	//	ngxData.GPWS_V1CallEnabled = data.GPWS_V1CallEnabled;
	//	js["GPWS_V1CallEnabled"] = data.GPWS_V1CallEnabled != 0;
	//}
	//if (ngxData.GroundConnAvailable != data.GroundConnAvailable) {
	//	ngxData.GroundConnAvailable = data.GroundConnAvailable;
	//	js["GroundConnAvailable"] = data.GroundConnAvailable != 0;
	//}
	//if (ngxData.FMC_TakeoffFlaps != data.FMC_TakeoffFlaps) {
	//	ngxData.FMC_TakeoffFlaps = data.FMC_TakeoffFlaps;
	//	js["FMC_TakeoffFlaps"] = data.FMC_TakeoffFlaps != 0;
	//}
	//if (ngxData.FMC_V1 != data.FMC_V1) {
	//	ngxData.FMC_V1 = data.FMC_V1;
	//	js["FMC_V1"] = data.FMC_V1 != 0;
	//}
	//if (ngxData.FMC_VR != data.FMC_VR) {
	//	ngxData.FMC_VR = data.FMC_VR;
	//	js["FMC_VR"] = data.FMC_VR != 0;
	//}
	//if (ngxData.FMC_V2 != data.FMC_V2) {
	//	ngxData.FMC_V2 = data.FMC_V2;
	//	js["FMC_V2"] = data.FMC_V2 != 0;
	//}
	//if (ngxData.FMC_LandingFlaps != data.FMC_LandingFlaps) {
	//	ngxData.FMC_LandingFlaps = data.FMC_LandingFlaps;
	//	js["FMC_LandingFlaps"] = data.FMC_LandingFlaps != 0;
	//}
	//if (ngxData.FMC_LandingVREF != data.FMC_LandingVREF) {
	//	ngxData.FMC_LandingVREF = data.FMC_LandingVREF;
	//	js["FMC_LandingVREF"] = data.FMC_LandingVREF != 0;
	//}
	//if (ngxData.FMC_CruiseAlt != data.FMC_CruiseAlt) {
	//	ngxData.FMC_CruiseAlt = data.FMC_CruiseAlt;
	//	js["FMC_CruiseAlt"] = data.FMC_CruiseAlt != 0;
	//}
	//if (ngxData.FMC_LandingAltitude != data.FMC_LandingAltitude) {
	//	ngxData.FMC_LandingAltitude = data.FMC_LandingAltitude;
	//	js["FMC_LandingAltitude"] = data.FMC_LandingAltitude != 0;
	//}
	//if (ngxData.FMC_TransitionAlt != data.FMC_TransitionAlt) {
	//	ngxData.FMC_TransitionAlt = data.FMC_TransitionAlt;
	//	js["FMC_TransitionAlt"] = data.FMC_TransitionAlt != 0;
	//}
	//if (ngxData.FMC_TransitionLevel != data.FMC_TransitionLevel) {
	//	ngxData.FMC_TransitionLevel = data.FMC_TransitionLevel;
	//	js["FMC_TransitionLevel"] = data.FMC_TransitionLevel != 0;
	//}
	//if (ngxData.FMC_PerfInputComplete != data.FMC_PerfInputComplete) {
	//	ngxData.FMC_PerfInputComplete = data.FMC_PerfInputComplete;
	//	js["FMC_PerfInputComplete"] = data.FMC_PerfInputComplete != 0;
	//}
	//if (ngxData.FMC_DistanceToTOD != data.FMC_DistanceToTOD) {
	//	ngxData.FMC_DistanceToTOD = data.FMC_DistanceToTOD;
	//	js["FMC_DistanceToTOD"] = data.FMC_DistanceToTOD != 0;
	//}
	//if (ngxData.FMC_DistanceToDest != data.FMC_DistanceToDest) {
	//	ngxData.FMC_DistanceToDest = data.FMC_DistanceToDest;
	//	js["FMC_DistanceToDest"] = data.FMC_DistanceToDest != 0;
	//}
	//if (ngxData.ICE_WindowHeatTestSw != data.ICE_WindowHeatTestSw) {
	//	ngxData.ICE_WindowHeatTestSw = data.ICE_WindowHeatTestSw;
	//	js["ICE_WindowHeatTestSw"] = data.ICE_WindowHeatTestSw != 0;
	//}


	if (ngxData.IRS_1_Switches_Status != data.IRS_1_Switches_Status) {
		ngxData.IRS_1_Switches_Status = data.IRS_1_Switches_Status;
		js["IRS_ModeSelector_1"] = data.IRS_1_Switches_Status != 0;
	}
	if (ngxData.IRS_2_Switches_Status != data.IRS_2_Switches_Status) {
		ngxData.IRS_2_Switches_Status = data.IRS_2_Switches_Status;
		js["IRS_ModeSelector_2"] = data.IRS_2_Switches_Status != 0;
	}
	if (ngxData.EEC_1_Switch_Status != data.EEC_1_Switch_Status) {
		ngxData.EEC_1_Switch_Status = data.EEC_1_Switch_Status;
		js["ENG_EECSwitch_1"] = data.EEC_1_Switch_Status != 0;
	}
	if (ngxData.EEC_2_Switch_Status != data.EEC_2_Switch_Status) {
		ngxData.EEC_2_Switch_Status = data.EEC_2_Switch_Status;
		js["ENG_EECSwitch_2"] = data.EEC_2_Switch_Status != 0;
	}
	if (ngxData.Flight_Control_A_Switches_Status != data.Flight_Control_A_Switches_Status) {
		ngxData.Flight_Control_A_Switches_Status = data.Flight_Control_A_Switches_Status;
		js["FCTL_FltControl_Sw_1"] = data.Flight_Control_A_Switches_Status;
	}
	if (ngxData.Flight_Control_B_Switches_Status != data.Flight_Control_B_Switches_Status) {
		ngxData.Flight_Control_B_Switches_Status = data.Flight_Control_B_Switches_Status;
		js["FCTL_FltControl_Sw_2"] = data.Flight_Control_B_Switches_Status;
	}
	if (ngxData.Spoiler_A_Switches_Status != data.Spoiler_A_Switches_Status) {
		ngxData.Spoiler_A_Switches_Status = data.Spoiler_A_Switches_Status;
		js["FCTL_Spoiler_Sw_1"] = data.Spoiler_A_Switches_Status != 0;
	}
	if (ngxData.Spoiler_B_Switches_Status != data.Spoiler_B_Switches_Status) {
		ngxData.Spoiler_B_Switches_Status = data.Spoiler_B_Switches_Status;
		js["FCTL_Spoiler_Sw_2"] = data.Spoiler_B_Switches_Status != 0;
	}
	if (ngxData.Fuel_L_FWD_Switch_Status != data.Fuel_L_FWD_Switch_Status) {
		ngxData.Fuel_L_FWD_Switch_Status = data.Fuel_L_FWD_Switch_Status;
		js["FUEL_PumpFwdSw_1"] = data.Fuel_L_FWD_Switch_Status != 0;
	}
	if (ngxData.Fuel_R_FWD_Switch_Status != data.Fuel_R_FWD_Switch_Status) {
		ngxData.Fuel_R_FWD_Switch_Status = data.Fuel_R_FWD_Switch_Status;
		js["FUEL_PumpFwdSw_2"] = data.Fuel_R_FWD_Switch_Status != 0;
	}
	if (ngxData.Fuel_L_AFT_Switch_Status != data.Fuel_L_AFT_Switch_Status) {
		ngxData.Fuel_L_AFT_Switch_Status = data.Fuel_L_AFT_Switch_Status;
		js["FUEL_PumpAftSw_1"] = data.Fuel_L_AFT_Switch_Status != 0;
	}
	if (ngxData.Fuel_R_AFT_Switch_Status != data.Fuel_R_AFT_Switch_Status) {
		ngxData.Fuel_R_AFT_Switch_Status = data.Fuel_R_AFT_Switch_Status;
		js["FUEL_PumpAftSw_2"] = data.Fuel_R_AFT_Switch_Status != 0;
	}
	if (ngxData.Fuel_CENTER_L_Switch_Status != data.Fuel_CENTER_L_Switch_Status) {
		ngxData.Fuel_CENTER_L_Switch_Status = data.Fuel_CENTER_L_Switch_Status;
		js["FUEL_PumpCtrSw_1"] = data.Fuel_CENTER_L_Switch_Status != 0;
	}
	if (ngxData.Fuel_CENTER_L_Switch_Status != data.Fuel_CENTER_L_Switch_Status) {
		ngxData.Fuel_CENTER_L_Switch_Status = data.Fuel_CENTER_L_Switch_Status;
		js["FUEL_PumpCtrSw_2"] = data.Fuel_CENTER_L_Switch_Status != 0;
	}
	if (ngxData.ELEC_IDGDisconnectSw[0] != data.ELEC_IDGDisconnectSw[0]) {
		ngxData.ELEC_IDGDisconnectSw[0] = data.ELEC_IDGDisconnectSw[0];
		js["ELEC_IDGDisconnectSw_1"] = data.ELEC_IDGDisconnectSw[0] != 0;
	}
	if (ngxData.ELEC_IDGDisconnectSw[1] != data.ELEC_IDGDisconnectSw[1]) {
		ngxData.ELEC_IDGDisconnectSw[1] = data.ELEC_IDGDisconnectSw[1];
		js["ELEC_IDGDisconnectSw_2"] = data.ELEC_IDGDisconnectSw[1] != 0;
	}
	//if (ngxData.ELEC_GenSw[0] != data.ELEC_GenSw[0]) {
	//	ngxData.ELEC_GenSw[0] = data.ELEC_GenSw[0];
	//	js["ELEC_GenSw_1"] = data.ELEC_GenSw[0] != 0;
	//}
	//if (ngxData.ELEC_GenSw[1] != data.ELEC_GenSw[1]) {
	//	ngxData.ELEC_GenSw[1] = data.ELEC_GenSw[1];
	//	js["ELEC_GenSw_2"] = data.ELEC_GenSw[1] != 0;
	//}
	if (ngxData.ELEC_APUGenSw[0] != data.ELEC_APUGenSw[0]) {
		ngxData.ELEC_APUGenSw[0] = data.ELEC_APUGenSw[0];
		js["ELEC_APUGenSw_1"] = data.ELEC_APUGenSw[0] != 0;
	}
	if (ngxData.ELEC_APUGenSw[1] != data.ELEC_APUGenSw[1]) {
		ngxData.ELEC_APUGenSw[1] = data.ELEC_APUGenSw[1];
		js["ELEC_APUGenSw_2"] = data.ELEC_APUGenSw[1] != 0;
	}
	if (ngxData.ICE_WindowHeatSw[0] != data.ICE_WindowHeatSw[0]) {
		ngxData.ICE_WindowHeatSw[0] = data.ICE_WindowHeatSw[0];
		js["ICE_WindowHeatSw_1"] = data.ICE_WindowHeatSw[0] != 0;
	}
	if (ngxData.ICE_WindowHeatSw[1] != data.ICE_WindowHeatSw[1]) {
		ngxData.ICE_WindowHeatSw[1] = data.ICE_WindowHeatSw[1];
		js["ICE_WindowHeatSw_2"] = data.ICE_WindowHeatSw[1] != 0;
	}
	if (ngxData.ICE_WindowHeatSw[2] != data.ICE_WindowHeatSw[2]) {
		ngxData.ICE_WindowHeatSw[2] = data.ICE_WindowHeatSw[2];
		js["ICE_WindowHeatSw_3"] = data.ICE_WindowHeatSw[2] != 0;
	}
	if (ngxData.ICE_WindowHeatSw[3] != data.ICE_WindowHeatSw[3]) {
		ngxData.ICE_WindowHeatSw[3] = data.ICE_WindowHeatSw[3];
		js["ICE_WindowHeatSw_4"] = data.ICE_WindowHeatSw[3] != 0;
	}

	if (ngxData.ICE_ProbeHeatSw[0] != data.ICE_ProbeHeatSw[0]) {
		ngxData.ICE_ProbeHeatSw[0] = data.ICE_ProbeHeatSw[0];
		js["ICE_ProbeHeatSw_1"] = data.ICE_ProbeHeatSw[0] != 0;
	}
	if (ngxData.ICE_ProbeHeatSw[1] != data.ICE_ProbeHeatSw[1]) {
		ngxData.ICE_ProbeHeatSw[1] = data.ICE_ProbeHeatSw[1];
		js["ICE_ProbeHeatSw_2"] = data.ICE_ProbeHeatSw[1] != 0;
	}
	if (ngxData.ICE_EngAntiIceSw[0] != data.ICE_EngAntiIceSw[0]) {
		ngxData.ICE_EngAntiIceSw[0] = data.ICE_EngAntiIceSw[0];
		js["ICE_EngAntiIceSw_1"] = data.ICE_EngAntiIceSw[0] != 0;
	}
	if (ngxData.ICE_EngAntiIceSw[1] != data.ICE_EngAntiIceSw[1]) {
		ngxData.ICE_EngAntiIceSw[1] = data.ICE_EngAntiIceSw[1];
		js["ICE_EngAntiIceSw_2"] = data.ICE_EngAntiIceSw[1] != 0;
	}
	if (ngxData.HYD_PumpSw_eng[0] != data.HYD_PumpSw_eng[0]) {
		ngxData.HYD_PumpSw_eng[0] = data.HYD_PumpSw_eng[0];
		js["HYD_PumpSw_eng_1"] = data.HYD_PumpSw_eng[0] != 0;
	}
	if (ngxData.HYD_PumpSw_eng[1] != data.HYD_PumpSw_eng[1]) {
		ngxData.HYD_PumpSw_eng[1] = data.HYD_PumpSw_eng[1];
		js["HYD_PumpSw_eng_2"] = data.HYD_PumpSw_eng[1] != 0;
	}
	if (ngxData.HYD_PumpSw_elec[0] != data.HYD_PumpSw_elec[0]) {
		ngxData.HYD_PumpSw_elec[0] = data.HYD_PumpSw_elec[0];
		js["HYD_PumpSw_elec_1"] = data.HYD_PumpSw_elec[0] != 0;
	}
	if (ngxData.HYD_PumpSw_elec[1] != data.HYD_PumpSw_elec[1]) {
		ngxData.HYD_PumpSw_elec[1] = data.HYD_PumpSw_elec[1];
		js["HYD_PumpSw_elec_2"] = data.HYD_PumpSw_elec[1] != 0;
	}
	if (ngxData.AIR_RecircFanSwitch[0] != data.AIR_RecircFanSwitch[0]) {
		ngxData.AIR_RecircFanSwitch[0] = data.AIR_RecircFanSwitch[0];
		js["AIR_RecircFanSwitch_1"] = data.AIR_RecircFanSwitch[0] != 0;
	}
	if (ngxData.AIR_RecircFanSwitch[1] != data.AIR_RecircFanSwitch[1]) {
		ngxData.AIR_RecircFanSwitch[1] = data.AIR_RecircFanSwitch[1];
		js["AIR_RecircFanSwitch_2"] = data.AIR_RecircFanSwitch[1] != 0;
	}
	if (ngxData.AIR_PackSwitch[0] != data.AIR_PackSwitch[0]) {
		ngxData.AIR_PackSwitch[0] = data.AIR_PackSwitch[0];
		js["AIR_PackSwitch_1"] = data.AIR_PackSwitch[0] != 0;
	}
	if (ngxData.AIR_PackSwitch[1] != data.AIR_PackSwitch[1]) {
		ngxData.AIR_PackSwitch[1] = data.AIR_PackSwitch[1];
		js["AIR_PackSwitch_2"] = data.AIR_PackSwitch[1] != 0;
	}
	if (ngxData.AIR_BleedAirSwitch[0] != data.AIR_BleedAirSwitch[0]) {
		ngxData.AIR_BleedAirSwitch[0] = data.AIR_BleedAirSwitch[0];
		js["AIR_BleedAirSwitch_1"] = data.AIR_BleedAirSwitch[0] != 0;
	}
	if (ngxData.AIR_BleedAirSwitch[1] != data.AIR_BleedAirSwitch[1]) {
		ngxData.AIR_BleedAirSwitch[1] = data.AIR_BleedAirSwitch[1];
		js["AIR_BleedAirSwitch_2"] = data.AIR_BleedAirSwitch[1] != 0;
	}
	if (ngxData.LTS_LandingLtRetractableSw[0] != data.LTS_LandingLtRetractableSw[0]) {
		ngxData.LTS_LandingLtRetractableSw[0] = data.LTS_LandingLtRetractableSw[0];
		js["LTS_LandingLtRetractableSw_1"] = data.LTS_LandingLtRetractableSw[0] != 0;
	}
	if (ngxData.LTS_LandingLtRetractableSw[1] != data.LTS_LandingLtRetractableSw[1]) {
		ngxData.LTS_LandingLtRetractableSw[1] = data.LTS_LandingLtRetractableSw[1];
		js["LTS_LandingLtRetractableSw_2"] = data.LTS_LandingLtRetractableSw[1] != 0;
	}
	if (ngxData.LTS_LandingLtFixedSw[0] != data.LTS_LandingLtFixedSw[0]) {
		ngxData.LTS_LandingLtFixedSw[0] = data.LTS_LandingLtFixedSw[0];
		js["LTS_LandingLtFixedSw_1"] = data.LTS_LandingLtFixedSw[0] != 0;
	}
	if (ngxData.LTS_LandingLtFixedSw[1] != data.LTS_LandingLtFixedSw[1]) {
		ngxData.LTS_LandingLtFixedSw[1] = data.LTS_LandingLtFixedSw[1];
		js["LTS_LandingLtFixedSw_2"] = data.LTS_LandingLtFixedSw[1] != 0;
	}
	if (ngxData.LTS_RunwayTurnoffSw[0] != data.LTS_RunwayTurnoffSw[0]) {
		ngxData.LTS_RunwayTurnoffSw[0] = data.LTS_RunwayTurnoffSw[0];
		js["LTS_RunwayTurnoffSw_1"] = data.LTS_RunwayTurnoffSw[0] != 0;
	}
	if (ngxData.LTS_RunwayTurnoffSw[1] != data.LTS_RunwayTurnoffSw[1]) {
		ngxData.LTS_RunwayTurnoffSw[1] = data.LTS_RunwayTurnoffSw[1];
		js["LTS_RunwayTurnoffSw_2"] = data.LTS_RunwayTurnoffSw[1] != 0;
	}
	if (ngxData.ENG_StartSelector[0] != data.ENG_StartSelector[0]) {
		ngxData.ENG_StartSelector[0] = data.ENG_StartSelector[0];
		js["ENG_StartSelector_1"] = data.ENG_StartSelector[0] != 0;
	}
	if (ngxData.ENG_StartSelector[1] != data.ENG_StartSelector[1]) {
		ngxData.ENG_StartSelector[1] = data.ENG_StartSelector[1];
		js["ENG_StartSelector_2"] = data.ENG_StartSelector[1] != 0;
	}
	//if (ngxData.EFIS_MinsSelBARO[0] != data.EFIS_MinsSelBARO[0]) {
	//	ngxData.EFIS_MinsSelBARO[0] = data.EFIS_MinsSelBARO[0];
	//	js["EFIS_MinsSelBARO_1"] = data.EFIS_MinsSelBARO[0] != 0;
	//}
	//if (ngxData.EFIS_MinsSelBARO[1] != data.EFIS_MinsSelBARO[1]) {
	//	ngxData.EFIS_MinsSelBARO[1] = data.EFIS_MinsSelBARO[1];
	//	js["EFIS_MinsSelBARO_2"] = data.EFIS_MinsSelBARO[1] != 0;
	//}
	//if (ngxData.EFIS_BaroSelHPA[0] != data.EFIS_BaroSelHPA[0]) {
	//	ngxData.EFIS_BaroSelHPA[0] = data.EFIS_BaroSelHPA[0];
	//	js["EFIS_BaroSelHPA_1"] = data.EFIS_BaroSelHPA[0] != 0;
	//}
	//if (ngxData.EFIS_BaroSelHPA[1] != data.EFIS_BaroSelHPA[1]) {
	//	ngxData.EFIS_BaroSelHPA[1] = data.EFIS_BaroSelHPA[1];
	//	js["EFIS_BaroSelHPA_2"] = data.EFIS_BaroSelHPA[1] != 0;
	//}
	if (ngxData.MAIN_MainPanelDUSel[0] != data.MAIN_MainPanelDUSel[0]) {
		ngxData.MAIN_MainPanelDUSel[0] = data.MAIN_MainPanelDUSel[0];
		js["MAIN_MainPanelDUSel_1"] = data.MAIN_MainPanelDUSel[0] != 0;
	}
	if (ngxData.MAIN_MainPanelDUSel[1] != data.MAIN_MainPanelDUSel[1]) {
		ngxData.MAIN_MainPanelDUSel[1] = data.MAIN_MainPanelDUSel[1];
		js["MAIN_MainPanelDUSel_2"] = data.MAIN_MainPanelDUSel[1] != 0;
	}
	if (ngxData.MAIN_LowerDUSel[0] != data.MAIN_LowerDUSel[0]) {
		ngxData.MAIN_LowerDUSel[0] = data.MAIN_LowerDUSel[0];
		js["MAIN_LowerDUSel_1"] = data.MAIN_LowerDUSel[0] != 0;
	}
	if (ngxData.MAIN_LowerDUSel[1] != data.MAIN_LowerDUSel[1]) {
		ngxData.MAIN_LowerDUSel[1] = data.MAIN_LowerDUSel[1];
		js["MAIN_LowerDUSel_2"] = data.MAIN_LowerDUSel[1] != 0;
	}
	//if (ngxData.MAIN_DisengageTestSelector[0] != data.MAIN_DisengageTestSelector[0]) {
	//	ngxData.MAIN_DisengageTestSelector[0] = data.MAIN_DisengageTestSelector[0];
	//	js["MAIN_DisengageTestSelector_1"] = data.MAIN_DisengageTestSelector[0] != 0;
	//}
	//if (ngxData.MAIN_DisengageTestSelector[1] != data.MAIN_DisengageTestSelector[1]) {
	//	ngxData.MAIN_DisengageTestSelector[1] = data.MAIN_DisengageTestSelector[1];
	//	js["MAIN_DisengageTestSelector_2"] = data.MAIN_DisengageTestSelector[1] != 0;
	//}
	if (ngxData.FIRE_OvhtDetSw[0] != data.FIRE_OvhtDetSw[0]) {
		ngxData.FIRE_OvhtDetSw[0] = data.FIRE_OvhtDetSw[0];
		js["FIRE_OvhtDetSw_1"] = data.FIRE_OvhtDetSw[0] != 0;
	}
	if (ngxData.FIRE_OvhtDetSw[1] != data.FIRE_OvhtDetSw[1]) {
		ngxData.FIRE_OvhtDetSw[1] = data.FIRE_OvhtDetSw[1];
		js["FIRE_OvhtDetSw_2"] = data.FIRE_OvhtDetSw[1] != 0;
	}
	//if (ngxData.FIRE_HandlePos[0] != data.FIRE_HandlePos[0]) {
	//	ngxData.FIRE_HandlePos[0] = data.FIRE_HandlePos[0];
	//	js["FIRE_HandlePos_1"] = data.FIRE_HandlePos[0] != 0;
	//}
	//if (ngxData.FIRE_HandlePos[1] != data.FIRE_HandlePos[1]) {
	//	ngxData.FIRE_HandlePos[1] = data.FIRE_HandlePos[1];
	//	js["FIRE_HandlePos_2"] = data.FIRE_HandlePos[1] != 0;
	//}
	//if (ngxData.FIRE_HandlePos[2] != data.FIRE_HandlePos[2]) {
	//	ngxData.FIRE_HandlePos[2] = data.FIRE_HandlePos[2];
	//	js["FIRE_HandlePos_3"] = data.FIRE_HandlePos[2] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[0] != data.FIRE_HandleIlluminated[0]) {
	//	ngxData.FIRE_HandleIlluminated[0] = data.FIRE_HandleIlluminated[0];
	//	js["FIRE_HandleIlluminated_1"] = data.FIRE_HandleIlluminated[0] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[1] != data.FIRE_HandleIlluminated[1]) {
	//	ngxData.FIRE_HandleIlluminated[1] = data.FIRE_HandleIlluminated[1];
	//	js["FIRE_HandleIlluminated_2"] = data.FIRE_HandleIlluminated[1] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[2] != data.FIRE_HandleIlluminated[2]) {
	//	ngxData.FIRE_HandleIlluminated[2] = data.FIRE_HandleIlluminated[2];
	//	js["FIRE_HandleIlluminated_3"] = data.FIRE_HandleIlluminated[2] != 0;
	//}
	if (ngxData.CARGO_DetSelect[0] != data.CARGO_DetSelect[0]) {
		ngxData.CARGO_DetSelect[0] = data.CARGO_DetSelect[0];
		js["CARGO_DetSelect_1"] = data.CARGO_DetSelect[0] != 0;
	}
	if (ngxData.CARGO_DetSelect[1] != data.CARGO_DetSelect[1]) {
		ngxData.CARGO_DetSelect[1] = data.CARGO_DetSelect[1];
		js["CARGO_DetSelect_2"] = data.CARGO_DetSelect[1] != 0;
	}
	//if (ngxData.CARGO_ArmedSw[0] != data.CARGO_ArmedSw[0]) {
	//	ngxData.CARGO_ArmedSw[0] = data.CARGO_ArmedSw[0];
	//	js["CARGO_ArmedSw_1"] = data.CARGO_ArmedSw[0] != 0;
	//}
	//if (ngxData.CARGO_ArmedSw[1] != data.CARGO_ArmedSw[1]) {
	//	ngxData.CARGO_ArmedSw[1] = data.CARGO_ArmedSw[1];
	//	js["CARGO_ArmedSw_2"] = data.CARGO_ArmedSw[1] != 0;
	//}
	if (ngxData.ENG_StartValve[0] != data.ENG_StartValve[0]) {
		ngxData.ENG_StartValve[0] = data.ENG_StartValve[0];
		js["ENG_StartValve_1"] = data.ENG_StartValve[0] != 0;
	}
	if (ngxData.ENG_StartValve[1] != data.ENG_StartValve[1]) {
		ngxData.ENG_StartValve[1] = data.ENG_StartValve[1];
		js["ENG_StartValve_2"] = data.ENG_StartValve[1] != 0;
	}
	//if (ngxData.COMM_SelectedMic[0] != data.COMM_SelectedMic[0]) {
	//	ngxData.COMM_SelectedMic[0] = data.COMM_SelectedMic[0];
	//	js["COMM_SelectedMic_1"] = data.COMM_SelectedMic[0] != 0;
	//}
	//if (ngxData.COMM_SelectedMic[1] != data.COMM_SelectedMic[1]) {
	//	ngxData.COMM_SelectedMic[1] = data.COMM_SelectedMic[1];
	//	js["COMM_SelectedMic_2"] = data.COMM_SelectedMic[1] != 0;
	//}
	//if (ngxData.COMM_SelectedMic[2] != data.COMM_SelectedMic[2]) {
	//	ngxData.COMM_SelectedMic[2] = data.COMM_SelectedMic[2];
	//	js["COMM_SelectedMic_3"] = data.COMM_SelectedMic[2] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[0] != data.COMM_ReceiverSwitches[0]) {
	//	ngxData.COMM_ReceiverSwitches[0] = data.COMM_ReceiverSwitches[0];
	//	js["COMM_ReceiverSwitches_1"] = data.COMM_ReceiverSwitches[0] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[1] != data.COMM_ReceiverSwitches[1]) {
	//	ngxData.COMM_ReceiverSwitches[1] = data.COMM_ReceiverSwitches[1];
	//	js["COMM_ReceiverSwitches_2"] = data.COMM_ReceiverSwitches[1] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[2] != data.COMM_ReceiverSwitches[2]) {
	//	ngxData.COMM_ReceiverSwitches[2] = data.COMM_ReceiverSwitches[2];
	//	js["COMM_ReceiverSwitches_3"] = data.COMM_ReceiverSwitches[2] != 0;
	//}


	if (ngxData.IRS_annunGPS != data.IRS_annunGPS) {
		ngxData.IRS_annunGPS = data.IRS_annunGPS;
		js["IRS_annunGPS"] = data.IRS_annunGPS != 0;
	}
	if (ngxData.WARN_annunPSEU != data.WARN_annunPSEU) {
		ngxData.WARN_annunPSEU = data.WARN_annunPSEU;
		js["WARN_annunPSEU"] = data.WARN_annunPSEU != 0;
	}
	if (ngxData.OXY_annunPASS_OXY_ON != data.OXY_annunPASS_OXY_ON) {
		ngxData.OXY_annunPASS_OXY_ON = data.OXY_annunPASS_OXY_ON;
		js["OXY_annunPASS_OXY_ON"] = data.OXY_annunPASS_OXY_ON != 0;
	}
	if (ngxData.GEAR_annunOvhdLEFT != data.GEAR_annunOvhdLEFT) {
		ngxData.GEAR_annunOvhdLEFT = data.GEAR_annunOvhdLEFT;
		js["GEAR_annunOvhdLEFT"] = data.GEAR_annunOvhdLEFT != 0;
	}
	if (ngxData.GEAR_annunOvhdNOSE != data.GEAR_annunOvhdNOSE) {
		ngxData.GEAR_annunOvhdNOSE = data.GEAR_annunOvhdNOSE;
		js["GEAR_annunOvhdNOSE"] = data.GEAR_annunOvhdNOSE != 0;
	}
	if (ngxData.GEAR_annunOvhdRIGHT != data.GEAR_annunOvhdRIGHT) {
		ngxData.GEAR_annunOvhdRIGHT = data.GEAR_annunOvhdRIGHT;
		js["GEAR_annunOvhdRIGHT"] = data.GEAR_annunOvhdRIGHT != 0;
	}
	//if (ngxData.FLTREC_annunOFF != data.FLTREC_annunOFF) {
	//	ngxData.FLTREC_annunOFF = data.FLTREC_annunOFF;
	//	js["FLTREC_annunOFF"] = data.FLTREC_annunOFF != 0;
	//}
	if (ngxData.FCTL_annunYAW_DAMPER != data.FCTL_annunYAW_DAMPER) {
		ngxData.FCTL_annunYAW_DAMPER = data.FCTL_annunYAW_DAMPER;
		js["FCTL_annunYAW_DAMPER"] = data.FCTL_annunYAW_DAMPER != 0;
	}
	if (ngxData.FCTL_annunLOW_QUANTITY != data.FCTL_annunLOW_QUANTITY) {
		ngxData.FCTL_annunLOW_QUANTITY = data.FCTL_annunLOW_QUANTITY;
		js["FCTL_annunLOW_QUANTITY"] = data.FCTL_annunLOW_QUANTITY != 0;
	}
	if (ngxData.FCTL_annunLOW_PRESSURE != data.FCTL_annunLOW_PRESSURE) {
		ngxData.FCTL_annunLOW_PRESSURE = data.FCTL_annunLOW_PRESSURE;
		js["FCTL_annunLOW_PRESSURE"] = data.FCTL_annunLOW_PRESSURE != 0;
	}
	if (ngxData.FCTL_annunLOW_STBY_RUD_ON != data.FCTL_annunLOW_STBY_RUD_ON) {
		ngxData.FCTL_annunLOW_STBY_RUD_ON = data.FCTL_annunLOW_STBY_RUD_ON;
		js["FCTL_annunLOW_STBY_RUD_ON"] = data.FCTL_annunLOW_STBY_RUD_ON != 0;
	}
	if (ngxData.FCTL_annunFEEL_DIFF_PRESS != data.FCTL_annunFEEL_DIFF_PRESS) {
		ngxData.FCTL_annunFEEL_DIFF_PRESS = data.FCTL_annunFEEL_DIFF_PRESS;
		js["FCTL_annunFEEL_DIFF_PRESS"] = data.FCTL_annunFEEL_DIFF_PRESS != 0;
	}
	if (ngxData.FCTL_annunSPEED_TRIM_FAIL != data.FCTL_annunSPEED_TRIM_FAIL) {
		ngxData.FCTL_annunSPEED_TRIM_FAIL = data.FCTL_annunSPEED_TRIM_FAIL;
		js["FCTL_annunSPEED_TRIM_FAIL"] = data.FCTL_annunSPEED_TRIM_FAIL != 0;
	}
	if (ngxData.FCTL_annunMACH_TRIM_FAIL != data.FCTL_annunMACH_TRIM_FAIL) {
		ngxData.FCTL_annunMACH_TRIM_FAIL = data.FCTL_annunMACH_TRIM_FAIL;
		js["FCTL_annunMACH_TRIM_FAIL"] = data.FCTL_annunMACH_TRIM_FAIL != 0;
	}
	if (ngxData.FCTL_annunAUTO_SLAT_FAIL != data.FCTL_annunAUTO_SLAT_FAIL) {
		ngxData.FCTL_annunAUTO_SLAT_FAIL = data.FCTL_annunAUTO_SLAT_FAIL;
		js["FCTL_annunAUTO_SLAT_FAIL"] = data.FCTL_annunAUTO_SLAT_FAIL != 0;
	}
	if (ngxData.FUEL_annunXFEED_VALVE_OPEN != data.FUEL_annunXFEED_VALVE_OPEN) {
		ngxData.FUEL_annunXFEED_VALVE_OPEN = data.FUEL_annunXFEED_VALVE_OPEN;
		js["FUEL_annunXFEED_VALVE_OPEN"] = data.FUEL_annunXFEED_VALVE_OPEN != 0;
	}
	if (ngxData.ELEC_annunBAT_DISCHARGE != data.ELEC_annunBAT_DISCHARGE) {
		ngxData.ELEC_annunBAT_DISCHARGE = data.ELEC_annunBAT_DISCHARGE;
		js["ELEC_annunBAT_DISCHARGE"] = data.ELEC_annunBAT_DISCHARGE != 0;
	}
	if (ngxData.ELEC_annunTR_UNIT != data.ELEC_annunTR_UNIT) {
		ngxData.ELEC_annunTR_UNIT = data.ELEC_annunTR_UNIT;
		js["ELEC_annunTR_UNIT"] = data.ELEC_annunTR_UNIT != 0;
	}
	if (ngxData.ELEC_annunELEC != data.ELEC_annunELEC) {
		ngxData.ELEC_annunELEC = data.ELEC_annunELEC;
		js["ELEC_annunELEC"] = data.ELEC_annunELEC != 0;
	}
	if (ngxData.ELEC_annunSTANDBY_POWER_OFF != data.ELEC_annunSTANDBY_POWER_OFF) {
		ngxData.ELEC_annunSTANDBY_POWER_OFF = data.ELEC_annunSTANDBY_POWER_OFF;
		js["ELEC_annunSTANDBY_POWER_OFF"] = data.ELEC_annunSTANDBY_POWER_OFF != 0;
	}
	if (ngxData.ELEC_annunGRD_POWER_AVAILABLE != data.ELEC_annunGRD_POWER_AVAILABLE) {
		ngxData.ELEC_annunGRD_POWER_AVAILABLE = data.ELEC_annunGRD_POWER_AVAILABLE;
		js["ELEC_annunGRD_POWER_AVAILABLE"] = data.ELEC_annunGRD_POWER_AVAILABLE != 0;
	}
	if (ngxData.ELEC_annunAPU_GEN_OFF_BUS != data.ELEC_annunAPU_GEN_OFF_BUS) {
		ngxData.ELEC_annunAPU_GEN_OFF_BUS = data.ELEC_annunAPU_GEN_OFF_BUS;
		js["ELEC_annunAPU_GEN_OFF_BUS"] = data.ELEC_annunAPU_GEN_OFF_BUS != 0;
	}
	//if (ngxData.APU_annunMAINT != data.APU_annunMAINT) {
	//	ngxData.APU_annunMAINT = data.APU_annunMAINT;
	//	js["APU_annunMAINT"] = data.APU_annunMAINT != 0;
	//}
	//if (ngxData.APU_annunLOW_OIL_PRESSURE != data.APU_annunLOW_OIL_PRESSURE) {
	//	ngxData.APU_annunLOW_OIL_PRESSURE = data.APU_annunLOW_OIL_PRESSURE;
	//	js["APU_annunLOW_OIL_PRESSURE"] = data.APU_annunLOW_OIL_PRESSURE != 0;
	//}
	//if (ngxData.APU_annunFAULT != data.APU_annunFAULT) {
	//	ngxData.APU_annunFAULT = data.APU_annunFAULT;
	//	js["APU_annunFAULT"] = data.APU_annunFAULT != 0;
	//}
	//if (ngxData.APU_annunOVERSPEED != data.APU_annunOVERSPEED) {
	//	ngxData.APU_annunOVERSPEED = data.APU_annunOVERSPEED;
	//	js["APU_annunOVERSPEED"] = data.APU_annunOVERSPEED != 0;
	//}
	if (ngxData.AIR_annunEquipCoolingSupplyOFF != data.AIR_annunEquipCoolingSupplyOFF) {
		ngxData.AIR_annunEquipCoolingSupplyOFF = data.AIR_annunEquipCoolingSupplyOFF;
		js["AIR_annunEquipCoolingSupplyOFF"] = data.AIR_annunEquipCoolingSupplyOFF != 0;
	}
	if (ngxData.AIR_annunEquipCoolingExhaustOFF != data.AIR_annunEquipCoolingExhaustOFF) {
		ngxData.AIR_annunEquipCoolingExhaustOFF = data.AIR_annunEquipCoolingExhaustOFF;
		js["AIR_annunEquipCoolingExhaustOFF"] = data.AIR_annunEquipCoolingExhaustOFF != 0;
	}
	if (ngxData.LTS_annunEmerNOT_ARMED != data.LTS_annunEmerNOT_ARMED) {
		ngxData.LTS_annunEmerNOT_ARMED = data.LTS_annunEmerNOT_ARMED;
		js["LTS_annunEmerNOT_ARMED"] = data.LTS_annunEmerNOT_ARMED != 0;
	}
	//if (ngxData.COMM_annunCALL != data.COMM_annunCALL) {
	//	ngxData.COMM_annunCALL = data.COMM_annunCALL;
	//	js["COMM_annunCALL"] = data.COMM_annunCALL != 0;
	//}
	//if (ngxData.COMM_annunPA_IN_USE != data.COMM_annunPA_IN_USE) {
	//	ngxData.COMM_annunPA_IN_USE = data.COMM_annunPA_IN_USE;
	//	js["COMM_annunPA_IN_USE"] = data.COMM_annunPA_IN_USE != 0;
	//}
	if (ngxData.ICE_annunCAPT_PITOT != data.ICE_annunCAPT_PITOT) {
		ngxData.ICE_annunCAPT_PITOT = data.ICE_annunCAPT_PITOT;
		js["ICE_annunCAPT_PITOT"] = data.ICE_annunCAPT_PITOT != 0;
	}
	if (ngxData.ICE_annunL_ELEV_PITOT != data.ICE_annunL_ELEV_PITOT) {
		ngxData.ICE_annunL_ELEV_PITOT = data.ICE_annunL_ELEV_PITOT;
		js["ICE_annunL_ELEV_PITOT"] = data.ICE_annunL_ELEV_PITOT != 0;
	}
	if (ngxData.ICE_annunL_ALPHA_VANE != data.ICE_annunL_ALPHA_VANE) {
		ngxData.ICE_annunL_ALPHA_VANE = data.ICE_annunL_ALPHA_VANE;
		js["ICE_annunL_ALPHA_VANE"] = data.ICE_annunL_ALPHA_VANE != 0;
	}
	if (ngxData.ICE_annunL_TEMP_PROBE != data.ICE_annunL_TEMP_PROBE) {
		ngxData.ICE_annunL_TEMP_PROBE = data.ICE_annunL_TEMP_PROBE;
		js["ICE_annunL_TEMP_PROBE"] = data.ICE_annunL_TEMP_PROBE != 0;
	}
	if (ngxData.ICE_annunFO_PITOT != data.ICE_annunFO_PITOT) {
		ngxData.ICE_annunFO_PITOT = data.ICE_annunFO_PITOT;
		js["ICE_annunFO_PITOT"] = data.ICE_annunFO_PITOT != 0;
	}
	if (ngxData.ICE_annunR_ELEV_PITOT != data.ICE_annunR_ELEV_PITOT) {
		ngxData.ICE_annunR_ELEV_PITOT = data.ICE_annunR_ELEV_PITOT;
		js["ICE_annunR_ELEV_PITOT"] = data.ICE_annunR_ELEV_PITOT != 0;
	}
	if (ngxData.ICE_annunR_ALPHA_VANE != data.ICE_annunR_ALPHA_VANE) {
		ngxData.ICE_annunR_ALPHA_VANE = data.ICE_annunR_ALPHA_VANE;
		js["ICE_annunR_ALPHA_VANE"] = data.ICE_annunR_ALPHA_VANE != 0;
	}
	if (ngxData.ICE_annunAUX_PITOT != data.ICE_annunAUX_PITOT) {
		ngxData.ICE_annunAUX_PITOT = data.ICE_annunAUX_PITOT;
		js["ICE_annunAUX_PITOT"] = data.ICE_annunAUX_PITOT != 0;
	}
	if (ngxData.AIR_annunDualBleed != data.AIR_annunDualBleed) {
		ngxData.AIR_annunDualBleed = data.AIR_annunDualBleed;
		js["AIR_annunDualBleed"] = data.AIR_annunDualBleed != 0;
	}
	if (ngxData.AIR_annunRamDoorL != data.AIR_annunRamDoorL) {
		ngxData.AIR_annunRamDoorL = data.AIR_annunRamDoorL;
		js["AIR_annunRamDoorL"] = data.AIR_annunRamDoorL != 0;
	}
	if (ngxData.AIR_annunRamDoorR != data.AIR_annunRamDoorR) {
		ngxData.AIR_annunRamDoorR = data.AIR_annunRamDoorR;
		js["AIR_annunRamDoorR"] = data.AIR_annunRamDoorR != 0;
	}
	//if (ngxData.AIR_FltAltWindow != data.AIR_FltAltWindow) {
	//	ngxData.AIR_FltAltWindow = data.AIR_FltAltWindow;
	//	js["AIR_FltAltWindow"] = data.AIR_FltAltWindow != 0;
	//}
	//if (ngxData.AIR_LandAltWindow != data.AIR_LandAltWindow) {
	//	ngxData.AIR_LandAltWindow = data.AIR_LandAltWindow;
	//	js["AIR_LandAltWindow"] = data.AIR_LandAltWindow != 0;
	//}
	if (ngxData.WARN_annunFLT_CONT != data.WARN_annunFLT_CONT) {
		ngxData.WARN_annunFLT_CONT = data.WARN_annunFLT_CONT;
		js["WARN_annunFLT_CONT"] = data.WARN_annunFLT_CONT != 0;
	}
	if (ngxData.WARN_annunIRS != data.WARN_annunIRS) {
		ngxData.WARN_annunIRS = data.WARN_annunIRS;
		js["WARN_annunIRS"] = data.WARN_annunIRS != 0;
	}
	if (ngxData.WARN_annunFUEL != data.WARN_annunFUEL) {
		ngxData.WARN_annunFUEL = data.WARN_annunFUEL;
		js["WARN_annunFUEL"] = data.WARN_annunFUEL != 0;
	}
	if (ngxData.WARN_annunELEC != data.WARN_annunELEC) {
		ngxData.WARN_annunELEC = data.WARN_annunELEC;
		js["WARN_annunELEC"] = data.WARN_annunELEC != 0;
	}
	if (ngxData.WARN_annunAPU != data.WARN_annunAPU) {
		ngxData.WARN_annunAPU = data.WARN_annunAPU;
		js["WARN_annunAPU"] = data.WARN_annunAPU != 0;
	}
	if (ngxData.WARN_annunOVHT_DET != data.WARN_annunOVHT_DET) {
		ngxData.WARN_annunOVHT_DET = data.WARN_annunOVHT_DET;
		js["WARN_annunOVHT_DET"] = data.WARN_annunOVHT_DET != 0;
	}
	if (ngxData.WARN_annunANTI_ICE != data.WARN_annunANTI_ICE) {
		ngxData.WARN_annunANTI_ICE = data.WARN_annunANTI_ICE;
		js["WARN_annunANTI_ICE"] = data.WARN_annunANTI_ICE != 0;
	}
	if (ngxData.WARN_annunHYD != data.WARN_annunHYD) {
		ngxData.WARN_annunHYD = data.WARN_annunHYD;
		js["WARN_annunHYD"] = data.WARN_annunHYD != 0;
	}
	if (ngxData.WARN_annunDOORS != data.WARN_annunDOORS) {
		ngxData.WARN_annunDOORS = data.WARN_annunDOORS;
		js["WARN_annunDOORS"] = data.WARN_annunDOORS != 0;
	}
	if (ngxData.WARN_annunENG != data.WARN_annunENG) {
		ngxData.WARN_annunENG = data.WARN_annunENG;
		js["WARN_annunENG"] = data.WARN_annunENG != 0;
	}
	if (ngxData.WARN_annunOVERHEAD != data.WARN_annunOVERHEAD) {
		ngxData.WARN_annunOVERHEAD = data.WARN_annunOVERHEAD;
		js["WARN_annunOVERHEAD"] = data.WARN_annunOVERHEAD != 0;
	}
	if (ngxData.WARN_annunAIR_COND != data.WARN_annunAIR_COND) {
		ngxData.WARN_annunAIR_COND = data.WARN_annunAIR_COND;
		js["WARN_annunAIR_COND"] = data.WARN_annunAIR_COND != 0;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_ARMED != data.MAIN_annunSPEEDBRAKE_ARMED) {
		ngxData.MAIN_annunSPEEDBRAKE_ARMED = data.MAIN_annunSPEEDBRAKE_ARMED;
		js["MAIN_annunSPEEDBRAKE_ARMED"] = data.MAIN_annunSPEEDBRAKE_ARMED != 0;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM != data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM) {
		ngxData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM;
		js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM != 0;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_EXTENDED != data.MAIN_annunSPEEDBRAKE_EXTENDED) {
		ngxData.MAIN_annunSPEEDBRAKE_EXTENDED = data.MAIN_annunSPEEDBRAKE_EXTENDED;
		js["MAIN_annunSPEEDBRAKE_EXTENDED"] = data.MAIN_annunSPEEDBRAKE_EXTENDED != 0;
	}
	if (ngxData.MAIN_annunSTAB_OUT_OF_TRIM != data.MAIN_annunSTAB_OUT_OF_TRIM) {
		ngxData.MAIN_annunSTAB_OUT_OF_TRIM = data.MAIN_annunSTAB_OUT_OF_TRIM;
		js["MAIN_annunSTAB_OUT_OF_TRIM"] = data.MAIN_annunSTAB_OUT_OF_TRIM != 0;
	}
	if (ngxData.MAIN_annunANTI_SKID_INOP != data.MAIN_annunANTI_SKID_INOP) {
		ngxData.MAIN_annunANTI_SKID_INOP = data.MAIN_annunANTI_SKID_INOP;
		js["MAIN_annunANTI_SKID_INOP"] = data.MAIN_annunANTI_SKID_INOP != 0;
	}
	if (ngxData.MAIN_annunAUTO_BRAKE_DISARM != data.MAIN_annunAUTO_BRAKE_DISARM) {
		ngxData.MAIN_annunAUTO_BRAKE_DISARM = data.MAIN_annunAUTO_BRAKE_DISARM;
		js["MAIN_annunAUTO_BRAKE_DISARM"] = data.MAIN_annunAUTO_BRAKE_DISARM != 0;
	}
	//if (ngxData.MAIN_annunLE_FLAPS_TRANSIT != data.MAIN_annunLE_FLAPS_TRANSIT) {
	//	ngxData.MAIN_annunLE_FLAPS_TRANSIT = data.MAIN_annunLE_FLAPS_TRANSIT;
	//	js["MAIN_annunLE_FLAPS_TRANSIT"] = data.MAIN_annunLE_FLAPS_TRANSIT != 0;
	//}
	//if (ngxData.MAIN_annunLE_FLAPS_EXT != data.MAIN_annunLE_FLAPS_EXT) {
	//	ngxData.MAIN_annunLE_FLAPS_EXT = data.MAIN_annunLE_FLAPS_EXT;
	//	js["MAIN_annunLE_FLAPS_EXT"] = data.MAIN_annunLE_FLAPS_EXT != 0;
	//}
	//if (ngxData.HGS_annun_AIII != data.HGS_annun_AIII) {
	//	ngxData.HGS_annun_AIII = data.HGS_annun_AIII;
	//	js["HGS_annun_AIII"] = data.HGS_annun_AIII != 0;
	//}
	//if (ngxData.HGS_annun_NO_AIII != data.HGS_annun_NO_AIII) {
	//	ngxData.HGS_annun_NO_AIII = data.HGS_annun_NO_AIII;
	//	js["HGS_annun_NO_AIII"] = data.HGS_annun_NO_AIII != 0;
	//}
	//if (ngxData.HGS_annun_FLARE != data.HGS_annun_FLARE) {
	//	ngxData.HGS_annun_FLARE = data.HGS_annun_FLARE;
	//	js["HGS_annun_FLARE"] = data.HGS_annun_FLARE != 0;
	//}
	//if (ngxData.HGS_annun_RO != data.HGS_annun_RO) {
	//	ngxData.HGS_annun_RO = data.HGS_annun_RO;
	//	js["HGS_annun_RO"] = data.HGS_annun_RO != 0;
	//}
	//if (ngxData.HGS_annun_RO_CTN != data.HGS_annun_RO_CTN) {
	//	ngxData.HGS_annun_RO_CTN = data.HGS_annun_RO_CTN;
	//	js["HGS_annun_RO_CTN"] = data.HGS_annun_RO_CTN != 0;
	//}
	//if (ngxData.HGS_annun_RO_ARM != data.HGS_annun_RO_ARM) {
	//	ngxData.HGS_annun_RO_ARM = data.HGS_annun_RO_ARM;
	//	js["HGS_annun_RO_ARM"] = data.HGS_annun_RO_ARM != 0;
	//}
	//if (ngxData.HGS_annun_TO != data.HGS_annun_TO) {
	//	ngxData.HGS_annun_TO = data.HGS_annun_TO;
	//	js["HGS_annun_TO"] = data.HGS_annun_TO != 0;
	//}
	//if (ngxData.HGS_annun_TO_CTN != data.HGS_annun_TO_CTN) {
	//	ngxData.HGS_annun_TO_CTN = data.HGS_annun_TO_CTN;
	//	js["HGS_annun_TO_CTN"] = data.HGS_annun_TO_CTN != 0;
	//}
	//if (ngxData.HGS_annun_APCH != data.HGS_annun_APCH) {
	//	ngxData.HGS_annun_APCH = data.HGS_annun_APCH;
	//	js["HGS_annun_APCH"] = data.HGS_annun_APCH != 0;
	//}
	//if (ngxData.HGS_annun_TO_WARN != data.HGS_annun_TO_WARN) {
	//	ngxData.HGS_annun_TO_WARN = data.HGS_annun_TO_WARN;
	//	js["HGS_annun_TO_WARN"] = data.HGS_annun_TO_WARN != 0;
	//}
	//if (ngxData.HGS_annun_Bar != data.HGS_annun_Bar) {
	//	ngxData.HGS_annun_Bar = data.HGS_annun_Bar;
	//	js["HGS_annun_Bar"] = data.HGS_annun_Bar != 0;
	//}
	//if (ngxData.HGS_annun_FAIL != data.HGS_annun_FAIL) {
	//	ngxData.HGS_annun_FAIL = data.HGS_annun_FAIL;
	//	js["HGS_annun_FAIL"] = data.HGS_annun_FAIL != 0;
	//}
	//if (ngxData.GPWS_annunINOP != data.GPWS_annunINOP) {
	//	ngxData.GPWS_annunINOP = data.GPWS_annunINOP;
	//	js["GPWS_annunINOP"] = data.GPWS_annunINOP != 0;
	//}
	if (ngxData.PED_annunParkingBrake != data.PED_annunParkingBrake) {
		ngxData.PED_annunParkingBrake = data.PED_annunParkingBrake;
		js["PED_annunParkingBrake"] = data.PED_annunParkingBrake != 0;
	}
	//if (ngxData.FIRE_annunWHEEL_WELL != data.FIRE_annunWHEEL_WELL) {
	//	ngxData.FIRE_annunWHEEL_WELL = data.FIRE_annunWHEEL_WELL;
	//	js["FIRE_annunWHEEL_WELL"] = data.FIRE_annunWHEEL_WELL != 0;
	//}
	//if (ngxData.FIRE_annunFAULT != data.FIRE_annunFAULT) {
	//	ngxData.FIRE_annunFAULT = data.FIRE_annunFAULT;
	//	js["FIRE_annunFAULT"] = data.FIRE_annunFAULT != 0;
	//}
	//if (ngxData.FIRE_annunAPU_DET_INOP != data.FIRE_annunAPU_DET_INOP) {
	//	ngxData.FIRE_annunAPU_DET_INOP = data.FIRE_annunAPU_DET_INOP;
	//	js["FIRE_annunAPU_DET_INOP"] = data.FIRE_annunAPU_DET_INOP != 0;
	//}
	//if (ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE != data.FIRE_annunAPU_BOTTLE_DISCHARGE) {
	//	ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE = data.FIRE_annunAPU_BOTTLE_DISCHARGE;
	//	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = data.FIRE_annunAPU_BOTTLE_DISCHARGE != 0;
	//}
	//if (ngxData.CARGO_annunFWD != data.CARGO_annunFWD) {
	//	ngxData.CARGO_annunFWD = data.CARGO_annunFWD;
	//	js["CARGO_annunFWD"] = data.CARGO_annunFWD != 0;
	//}
	//if (ngxData.CARGO_annunAFT != data.CARGO_annunAFT) {
	//	ngxData.CARGO_annunAFT = data.CARGO_annunAFT;
	//	js["CARGO_annunAFT"] = data.CARGO_annunAFT != 0;
	//}
	//if (ngxData.CARGO_annunDETECTOR_FAULT != data.CARGO_annunDETECTOR_FAULT) {
	//	ngxData.CARGO_annunDETECTOR_FAULT = data.CARGO_annunDETECTOR_FAULT;
	//	js["CARGO_annunDETECTOR_FAULT"] = data.CARGO_annunDETECTOR_FAULT != 0;
	//}
	//if (ngxData.CARGO_annunDISCH != data.CARGO_annunDISCH) {
	//	ngxData.CARGO_annunDISCH = data.CARGO_annunDISCH;
	//	js["CARGO_annunDISCH"] = data.CARGO_annunDISCH != 0;
	//}
	//if (ngxData.HGS_annunRWY != data.HGS_annunRWY) {
	//	ngxData.HGS_annunRWY = data.HGS_annunRWY;
	//	js["HGS_annunRWY"] = data.HGS_annunRWY != 0;
	//}
	//if (ngxData.HGS_annunGS != data.HGS_annunGS) {
	//	ngxData.HGS_annunGS = data.HGS_annunGS;
	//	js["HGS_annunGS"] = data.HGS_annunGS != 0;
	//}
	//if (ngxData.HGS_annunFAULT != data.HGS_annunFAULT) {
	//	ngxData.HGS_annunFAULT = data.HGS_annunFAULT;
	//	js["HGS_annunFAULT"] = data.HGS_annunFAULT != 0;
	//}
	//if (ngxData.HGS_annunCLR != data.HGS_annunCLR) {
	//	ngxData.HGS_annunCLR = data.HGS_annunCLR;
	//	js["HGS_annunCLR"] = data.HGS_annunCLR != 0;
	//}
	//if (ngxData.XPDR_annunFAIL != data.XPDR_annunFAIL) {
	//	ngxData.XPDR_annunFAIL = data.XPDR_annunFAIL;
	//	js["XPDR_annunFAIL"] = data.XPDR_annunFAIL != 0;
	//}
	//if (ngxData.PED_annunLOCK_FAIL != data.PED_annunLOCK_FAIL) {
	//	ngxData.PED_annunLOCK_FAIL = data.PED_annunLOCK_FAIL;
	//	js["PED_annunLOCK_FAIL"] = data.PED_annunLOCK_FAIL != 0;
	//}
	//if (ngxData.PED_annunAUTO_UNLK != data.PED_annunAUTO_UNLK) {
	//	ngxData.PED_annunAUTO_UNLK = data.PED_annunAUTO_UNLK;
	//	js["PED_annunAUTO_UNLK"] = data.PED_annunAUTO_UNLK != 0;
	//}
	//if (ngxData.DOOR_annunFWD_ENTRY != data.DOOR_annunFWD_ENTRY) {
	//	ngxData.DOOR_annunFWD_ENTRY = data.DOOR_annunFWD_ENTRY;
	//	js["DOOR_annunFWD_ENTRY"] = data.DOOR_annunFWD_ENTRY != 0;
	//}
	//if (ngxData.DOOR_annunFWD_SERVICE != data.DOOR_annunFWD_SERVICE) {
	//	ngxData.DOOR_annunFWD_SERVICE = data.DOOR_annunFWD_SERVICE;
	//	js["DOOR_annunFWD_SERVICE"] = data.DOOR_annunFWD_SERVICE != 0;
	//}
	//if (ngxData.DOOR_annunAIRSTAIR != data.DOOR_annunAIRSTAIR) {
	//	ngxData.DOOR_annunAIRSTAIR = data.DOOR_annunAIRSTAIR;
	//	js["DOOR_annunAIRSTAIR"] = data.DOOR_annunAIRSTAIR != 0;
	//}
	//if (ngxData.DOOR_annunLEFT_FWD_OVERWING != data.DOOR_annunLEFT_FWD_OVERWING) {
	//	ngxData.DOOR_annunLEFT_FWD_OVERWING = data.DOOR_annunLEFT_FWD_OVERWING;
	//	js["DOOR_annunLEFT_FWD_OVERWING"] = data.DOOR_annunLEFT_FWD_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunRIGHT_FWD_OVERWING != data.DOOR_annunRIGHT_FWD_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_FWD_OVERWING = data.DOOR_annunRIGHT_FWD_OVERWING;
	//	js["DOOR_annunRIGHT_FWD_OVERWING"] = data.DOOR_annunRIGHT_FWD_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunFWD_CARGO != data.DOOR_annunFWD_CARGO) {
	//	ngxData.DOOR_annunFWD_CARGO = data.DOOR_annunFWD_CARGO;
	//	js["DOOR_annunFWD_CARGO"] = data.DOOR_annunFWD_CARGO != 0;
	//}
	//if (ngxData.DOOR_annunEQUIP != data.DOOR_annunEQUIP) {
	//	ngxData.DOOR_annunEQUIP = data.DOOR_annunEQUIP;
	//	js["DOOR_annunEQUIP"] = data.DOOR_annunEQUIP != 0;
	//}
	//if (ngxData.DOOR_annunLEFT_AFT_OVERWING != data.DOOR_annunLEFT_AFT_OVERWING) {
	//	ngxData.DOOR_annunLEFT_AFT_OVERWING = data.DOOR_annunLEFT_AFT_OVERWING;
	//	js["DOOR_annunLEFT_AFT_OVERWING"] = data.DOOR_annunLEFT_AFT_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunRIGHT_AFT_OVERWING != data.DOOR_annunRIGHT_AFT_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_AFT_OVERWING = data.DOOR_annunRIGHT_AFT_OVERWING;
	//	js["DOOR_annunRIGHT_AFT_OVERWING"] = data.DOOR_annunRIGHT_AFT_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunAFT_CARGO != data.DOOR_annunAFT_CARGO) {
	//	ngxData.DOOR_annunAFT_CARGO = data.DOOR_annunAFT_CARGO;
	//	js["DOOR_annunAFT_CARGO"] = data.DOOR_annunAFT_CARGO != 0;
	//}
	//if (ngxData.DOOR_annunAFT_ENTRY != data.DOOR_annunAFT_ENTRY) {
	//	ngxData.DOOR_annunAFT_ENTRY = data.DOOR_annunAFT_ENTRY;
	//	js["DOOR_annunAFT_ENTRY"] = data.DOOR_annunAFT_ENTRY != 0;
	//}
	//if (ngxData.DOOR_annunAFT_SERVICE != data.DOOR_annunAFT_SERVICE) {
	//	ngxData.DOOR_annunAFT_SERVICE = data.DOOR_annunAFT_SERVICE;
	//	js["DOOR_annunAFT_SERVICE"] = data.DOOR_annunAFT_SERVICE != 0;
	//}
	if (ngxData.AIR_annunAUTO_FAIL != data.AIR_annunAUTO_FAIL) {
		ngxData.AIR_annunAUTO_FAIL = data.AIR_annunAUTO_FAIL;
		js["AIR_annunAUTO_FAIL"] = data.AIR_annunAUTO_FAIL != 0;
	}
	if (ngxData.AIR_annunOFFSCHED_DESCENT != data.AIR_annunOFFSCHED_DESCENT) {
		ngxData.AIR_annunOFFSCHED_DESCENT = data.AIR_annunOFFSCHED_DESCENT;
		js["AIR_annunOFFSCHED_DESCENT"] = data.AIR_annunOFFSCHED_DESCENT != 0;
	}
	if (ngxData.AIR_annunALTN != data.AIR_annunALTN) {
		ngxData.AIR_annunALTN = data.AIR_annunALTN;
		js["AIR_annunALTN"] = data.AIR_annunALTN != 0;
	}
	if (ngxData.AIR_annunMANUAL != data.AIR_annunMANUAL) {
		ngxData.AIR_annunMANUAL = data.AIR_annunMANUAL;
		js["AIR_annunMANUAL"] = data.AIR_annunMANUAL != 0;
	}



	//if (ngxData.IRS_annunALIGN[0] != data.IRS_annunALIGN[0]) {
	//	ngxData.IRS_annunALIGN[0] = data.IRS_annunALIGN[0];
	//	js["IRS_annunALIGN_1"] = data.IRS_annunALIGN[0] != 0;
	//}
	//if (ngxData.IRS_annunALIGN[1] != data.IRS_annunALIGN[1]) {
	//	ngxData.IRS_annunALIGN[1] = data.IRS_annunALIGN[1];
	//	js["IRS_annunALIGN_2"] = data.IRS_annunALIGN[1] != 0;
	//}
	//if (ngxData.IRS_annunON_DC[0] != data.IRS_annunON_DC[0]) {
	//	ngxData.IRS_annunON_DC[0] = data.IRS_annunON_DC[0];
	//	js["IRS_annunON_DC_1"] = data.IRS_annunON_DC[0] != 0;
	//}
	//if (ngxData.IRS_annunON_DC[1] != data.IRS_annunON_DC[1]) {
	//	ngxData.IRS_annunON_DC[1] = data.IRS_annunON_DC[1];
	//	js["IRS_annunON_DC_2"] = data.IRS_annunON_DC[1] != 0;
	//}
	//if (ngxData.IRS_annunFAULT[0] != data.IRS_annunFAULT[0]) {
	//	ngxData.IRS_annunFAULT[0] = data.IRS_annunFAULT[0];
	//	js["IRS_annunFAULT_1"] = data.IRS_annunFAULT[0] != 0;
	//}
	//if (ngxData.IRS_annunFAULT[1] != data.IRS_annunFAULT[1]) {
	//	ngxData.IRS_annunFAULT[1] = data.IRS_annunFAULT[1];
	//	js["IRS_annunFAULT_2"] = data.IRS_annunFAULT[1] != 0;
	//}
	//if (ngxData.IRS_annunDC_FAIL[0] != data.IRS_annunDC_FAIL[0]) {
	//	ngxData.IRS_annunDC_FAIL[0] = data.IRS_annunDC_FAIL[0];
	//	js["IRS_annunDC_FAIL_1"] = data.IRS_annunDC_FAIL[0] != 0;
	//}
	//if (ngxData.IRS_annunDC_FAIL[1] != data.IRS_annunDC_FAIL[1]) {
	//	ngxData.IRS_annunDC_FAIL[1] = data.IRS_annunDC_FAIL[1];
	//	js["IRS_annunDC_FAIL_2"] = data.IRS_annunDC_FAIL[1] != 0;
	//}
	if (ngxData.ENG_annunREVERSER[0] != data.ENG_annunREVERSER[0]) {
		ngxData.ENG_annunREVERSER[0] = data.ENG_annunREVERSER[0];
		js["ENG_annunREVERSER_1"] = data.ENG_annunREVERSER[0] != 0;
	}
	if (ngxData.ENG_annunREVERSER[1] != data.ENG_annunREVERSER[1]) {
		ngxData.ENG_annunREVERSER[1] = data.ENG_annunREVERSER[1];
		js["ENG_annunREVERSER_2"] = data.ENG_annunREVERSER[1] != 0;
	}
	if (ngxData.ENG_annunENGINE_CONTROL[0] != data.ENG_annunENGINE_CONTROL[0]) {
		ngxData.ENG_annunENGINE_CONTROL[0] = data.ENG_annunENGINE_CONTROL[0];
		js["ENG_annunENGINE_CONTROL_1"] = data.ENG_annunENGINE_CONTROL[0] != 0;
	}
	if (ngxData.ENG_annunENGINE_CONTROL[1] != data.ENG_annunENGINE_CONTROL[1]) {
		ngxData.ENG_annunENGINE_CONTROL[1] = data.ENG_annunENGINE_CONTROL[1];
		js["ENG_annunENGINE_CONTROL_2"] = data.ENG_annunENGINE_CONTROL[1] != 0;
	}
	//if (ngxData.ENG_annunALTN[0] != data.ENG_annunALTN[0]) {
	//	ngxData.ENG_annunALTN[0] = data.ENG_annunALTN[0];
	//	js["ENG_annunALTN_1"] = data.ENG_annunALTN[0] != 0;
	//}
	//if (ngxData.ENG_annunALTN[1] != data.ENG_annunALTN[1]) {
	//	ngxData.ENG_annunALTN[1] = data.ENG_annunALTN[1];
	//	js["ENG_annunALTN_2"] = data.ENG_annunALTN[1] != 0;
	//}
	if (ngxData.FCTL_annunFC_LOW_PRESSURE[0] != data.FCTL_annunFC_LOW_PRESSURE[0]) {
		ngxData.FCTL_annunFC_LOW_PRESSURE[0] = data.FCTL_annunFC_LOW_PRESSURE[0];
		js["FCTL_annunFC_LOW_PRESSURE_1"] = data.FCTL_annunFC_LOW_PRESSURE[0] != 0;
	}
	if (ngxData.FCTL_annunFC_LOW_PRESSURE[1] != data.FCTL_annunFC_LOW_PRESSURE[1]) {
		ngxData.FCTL_annunFC_LOW_PRESSURE[1] = data.FCTL_annunFC_LOW_PRESSURE[1];
		js["FCTL_annunFC_LOW_PRESSURE_2"] = data.FCTL_annunFC_LOW_PRESSURE[1] != 0;
	}
	if (ngxData.FUEL_annunENG_VALVE_CLOSED[0] != data.FUEL_annunENG_VALVE_CLOSED[0]) {
		ngxData.FUEL_annunENG_VALVE_CLOSED[0] = data.FUEL_annunENG_VALVE_CLOSED[0];
#ifdef PMDG_737_NG3 
		js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0] != 0 != 0;
#else
		js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0] != 0;

#endif
	}
	if (ngxData.FUEL_annunENG_VALVE_CLOSED[1] != data.FUEL_annunENG_VALVE_CLOSED[1]) {
		ngxData.FUEL_annunENG_VALVE_CLOSED[1] = data.FUEL_annunENG_VALVE_CLOSED[1];
#ifdef PMDG_737_NG3 
		js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1] != 0 != 0;
#else
		js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1] != 0;
#endif
	}
	if (ngxData.FUEL_annunSPAR_VALVE_CLOSED[0] != data.FUEL_annunSPAR_VALVE_CLOSED[0]) {
		ngxData.FUEL_annunSPAR_VALVE_CLOSED[0] = data.FUEL_annunSPAR_VALVE_CLOSED[0];
#ifdef PMDG_737_NG3 
		js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0] != 0 != 0;
#else
		js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0] != 0;
#endif
	}
	if (ngxData.FUEL_annunSPAR_VALVE_CLOSED[1] != data.FUEL_annunSPAR_VALVE_CLOSED[1]) {
		ngxData.FUEL_annunSPAR_VALVE_CLOSED[1] = data.FUEL_annunSPAR_VALVE_CLOSED[1];
#ifdef PMDG_737_NG3 
		js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1] != 0 != 0;
#else
		js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1] != 0;
#endif
	}
	if (ngxData.FUEL_annunFILTER_BYPASS[0] != data.FUEL_annunFILTER_BYPASS[0]) {
		ngxData.FUEL_annunFILTER_BYPASS[0] = data.FUEL_annunFILTER_BYPASS[0];
		js["FUEL_annunFILTER_BYPASS_1"] = data.FUEL_annunFILTER_BYPASS[0] != 0;
	}
	if (ngxData.FUEL_annunFILTER_BYPASS[1] != data.FUEL_annunFILTER_BYPASS[1]) {
		ngxData.FUEL_annunFILTER_BYPASS[1] = data.FUEL_annunFILTER_BYPASS[1];
		js["FUEL_annunFILTER_BYPASS_2"] = data.FUEL_annunFILTER_BYPASS[1] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Fwd[0] != data.FUEL_annunLOWPRESS_Fwd[0]) {
		ngxData.FUEL_annunLOWPRESS_Fwd[0] = data.FUEL_annunLOWPRESS_Fwd[0];
		js["FUEL_annunLOWPRESS_Fwd_1"] = data.FUEL_annunLOWPRESS_Fwd[0] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Fwd[1] != data.FUEL_annunLOWPRESS_Fwd[1]) {
		ngxData.FUEL_annunLOWPRESS_Fwd[1] = data.FUEL_annunLOWPRESS_Fwd[1];
		js["FUEL_annunLOWPRESS_Fwd_2"] = data.FUEL_annunLOWPRESS_Fwd[1] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Aft[0] != data.FUEL_annunLOWPRESS_Aft[0]) {
		ngxData.FUEL_annunLOWPRESS_Aft[0] = data.FUEL_annunLOWPRESS_Aft[0];
		js["FUEL_annunLOWPRESS_Aft_1"] = data.FUEL_annunLOWPRESS_Aft[0] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Aft[1] != data.FUEL_annunLOWPRESS_Aft[1]) {
		ngxData.FUEL_annunLOWPRESS_Aft[1] = data.FUEL_annunLOWPRESS_Aft[1];
		js["FUEL_annunLOWPRESS_Aft_2"] = data.FUEL_annunLOWPRESS_Aft[1] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Ctr[0] != data.FUEL_annunLOWPRESS_Ctr[0]) {
		ngxData.FUEL_annunLOWPRESS_Ctr[0] = data.FUEL_annunLOWPRESS_Ctr[0];
		js["FUEL_annunLOWPRESS_Ctr_1"] = data.FUEL_annunLOWPRESS_Ctr[0] != 0;
	}
	if (ngxData.FUEL_annunLOWPRESS_Ctr[1] != data.FUEL_annunLOWPRESS_Ctr[1]) {
		ngxData.FUEL_annunLOWPRESS_Ctr[1] = data.FUEL_annunLOWPRESS_Ctr[1];
		js["FUEL_annunLOWPRESS_Ctr_2"] = data.FUEL_annunLOWPRESS_Ctr[1] != 0;
	}
	if (ngxData.ELEC_annunDRIVE[0] != data.ELEC_annunDRIVE[0]) {
		ngxData.ELEC_annunDRIVE[0] = data.ELEC_annunDRIVE[0];
		js["ELEC_annunDRIVE_1"] = data.ELEC_annunDRIVE[0] != 0;
	}
	if (ngxData.ELEC_annunDRIVE[1] != data.ELEC_annunDRIVE[1]) {
		ngxData.ELEC_annunDRIVE[1] = data.ELEC_annunDRIVE[1];
		js["ELEC_annunDRIVE_2"] = data.ELEC_annunDRIVE[1] != 0;
	}
	if (ngxData.ELEC_annunTRANSFER_BUS_OFF[0] != data.ELEC_annunTRANSFER_BUS_OFF[0]) {
		ngxData.ELEC_annunTRANSFER_BUS_OFF[0] = data.ELEC_annunTRANSFER_BUS_OFF[0];
		js["ELEC_annunTRANSFER_BUS_OFF_1"] = data.ELEC_annunTRANSFER_BUS_OFF[0] != 0;
	}
	if (ngxData.ELEC_annunTRANSFER_BUS_OFF[1] != data.ELEC_annunTRANSFER_BUS_OFF[1]) {
		ngxData.ELEC_annunTRANSFER_BUS_OFF[1] = data.ELEC_annunTRANSFER_BUS_OFF[1];
		js["ELEC_annunTRANSFER_BUS_OFF_2"] = data.ELEC_annunTRANSFER_BUS_OFF[1] != 0;
	}
	if (ngxData.ELEC_annunSOURCE_OFF[0] != data.ELEC_annunSOURCE_OFF[0]) {
		ngxData.ELEC_annunSOURCE_OFF[0] = data.ELEC_annunSOURCE_OFF[0];
		js["ELEC_annunSOURCE_OFF_1"] = data.ELEC_annunSOURCE_OFF[0] != 0;
	}
	if (ngxData.ELEC_annunSOURCE_OFF[1] != data.ELEC_annunSOURCE_OFF[1]) {
		ngxData.ELEC_annunSOURCE_OFF[1] = data.ELEC_annunSOURCE_OFF[1];
		js["ELEC_annunSOURCE_OFF_2"] = data.ELEC_annunSOURCE_OFF[1] != 0;
	}
	if (ngxData.ELEC_annunGEN_BUS_OFF[0] != data.ELEC_annunGEN_BUS_OFF[0]) {
		ngxData.ELEC_annunGEN_BUS_OFF[0] = data.ELEC_annunGEN_BUS_OFF[0];
		js["ELEC_annunGEN_BUS_OFF_1"] = data.ELEC_annunGEN_BUS_OFF[0] != 0;
	}
	if (ngxData.ELEC_annunGEN_BUS_OFF[1] != data.ELEC_annunGEN_BUS_OFF[1]) {
		ngxData.ELEC_annunGEN_BUS_OFF[1] = data.ELEC_annunGEN_BUS_OFF[1];
		js["ELEC_annunGEN_BUS_OFF_2"] = data.ELEC_annunGEN_BUS_OFF[1] != 0;
	}
	if (ngxData.ICE_annunOVERHEAT[0] != data.ICE_annunOVERHEAT[0]) {
		ngxData.ICE_annunOVERHEAT[0] = data.ICE_annunOVERHEAT[0];
		js["ICE_annunOVERHEAT_1"] = data.ICE_annunOVERHEAT[0] != 0;
	}
	if (ngxData.ICE_annunOVERHEAT[1] != data.ICE_annunOVERHEAT[1]) {
		ngxData.ICE_annunOVERHEAT[1] = data.ICE_annunOVERHEAT[1];
		js["ICE_annunOVERHEAT_2"] = data.ICE_annunOVERHEAT[1] != 0;
	}
	if (ngxData.ICE_annunOVERHEAT[2] != data.ICE_annunOVERHEAT[2]) {
		ngxData.ICE_annunOVERHEAT[2] = data.ICE_annunOVERHEAT[2];
		js["ICE_annunOVERHEAT_3"] = data.ICE_annunOVERHEAT[2] != 0;
	}
	if (ngxData.ICE_annunOVERHEAT[3] != data.ICE_annunOVERHEAT[3]) {
		ngxData.ICE_annunOVERHEAT[3] = data.ICE_annunOVERHEAT[3];
		js["ICE_annunOVERHEAT_4"] = data.ICE_annunOVERHEAT[3] != 0;
	}
	if (ngxData.ICE_annunON[0] != data.ICE_annunON[0]) {
		ngxData.ICE_annunON[0] = data.ICE_annunON[0];
		js["ICE_annunON_1"] = data.ICE_annunON[0] != 0;
	}
	if (ngxData.ICE_annunON[1] != data.ICE_annunON[1]) {
		ngxData.ICE_annunON[1] = data.ICE_annunON[1];
		js["ICE_annunON_2"] = data.ICE_annunON[1] != 0;
	}
	if (ngxData.ICE_annunON[2] != data.ICE_annunON[2]) {
		ngxData.ICE_annunON[2] = data.ICE_annunON[2];
		js["ICE_annunON_3"] = data.ICE_annunON[2] != 0;
	}
	if (ngxData.ICE_annunON[3] != data.ICE_annunON[3]) {
		ngxData.ICE_annunON[3] = data.ICE_annunON[3];
		js["ICE_annunON_4"] = data.ICE_annunON[3] != 0;
	}
	if (ngxData.ICE_annunVALVE_OPEN[0] != data.ICE_annunVALVE_OPEN[0]) {
		ngxData.ICE_annunVALVE_OPEN[0] = data.ICE_annunVALVE_OPEN[0];
		js["ICE_annunVALVE_OPEN_1"] = data.ICE_annunVALVE_OPEN[0] != 0;
	}
	if (ngxData.ICE_annunVALVE_OPEN[1] != data.ICE_annunVALVE_OPEN[1]) {
		ngxData.ICE_annunVALVE_OPEN[1] = data.ICE_annunVALVE_OPEN[1];
		js["ICE_annunVALVE_OPEN_2"] = data.ICE_annunVALVE_OPEN[1] != 0;
	}
	if (ngxData.ICE_annunCOWL_ANTI_ICE[0] != data.ICE_annunCOWL_ANTI_ICE[0]) {
		ngxData.ICE_annunCOWL_ANTI_ICE[0] = data.ICE_annunCOWL_ANTI_ICE[0];
		js["ICE_annunCOWL_ANTI_ICE_1"] = data.ICE_annunCOWL_ANTI_ICE[0] != 0;
	}
	if (ngxData.ICE_annunCOWL_ANTI_ICE[1] != data.ICE_annunCOWL_ANTI_ICE[1]) {
		ngxData.ICE_annunCOWL_ANTI_ICE[1] = data.ICE_annunCOWL_ANTI_ICE[1];
		js["ICE_annunCOWL_ANTI_ICE_2"] = data.ICE_annunCOWL_ANTI_ICE[1] != 0;
	}
	if (ngxData.ICE_annunCOWL_VALVE_OPEN[0] != data.ICE_annunCOWL_VALVE_OPEN[0]) {
		ngxData.ICE_annunCOWL_VALVE_OPEN[0] = data.ICE_annunCOWL_VALVE_OPEN[0];
		js["ICE_annunCOWL_VALVE_OPEN_1"] = data.ICE_annunCOWL_VALVE_OPEN[0] != 0;
	}
	if (ngxData.ICE_annunCOWL_VALVE_OPEN[1] != data.ICE_annunCOWL_VALVE_OPEN[1]) {
		ngxData.ICE_annunCOWL_VALVE_OPEN[1] = data.ICE_annunCOWL_VALVE_OPEN[1];
		js["ICE_annunCOWL_VALVE_OPEN_2"] = data.ICE_annunCOWL_VALVE_OPEN[1] != 0;
	}
	if (ngxData.HYD_annunLOW_PRESS_eng[0] != data.HYD_annunLOW_PRESS_eng[0]) {
		ngxData.HYD_annunLOW_PRESS_eng[0] = data.HYD_annunLOW_PRESS_eng[0];
		js["HYD_annunLOW_PRESS_eng_1"] = data.HYD_annunLOW_PRESS_eng[0] != 0;
	}
	if (ngxData.HYD_annunLOW_PRESS_eng[1] != data.HYD_annunLOW_PRESS_eng[1]) {
		ngxData.HYD_annunLOW_PRESS_eng[1] = data.HYD_annunLOW_PRESS_eng[1];
		js["HYD_annunLOW_PRESS_eng_2"] = data.HYD_annunLOW_PRESS_eng[1] != 0;
	}
	if (ngxData.HYD_annunLOW_PRESS_elec[0] != data.HYD_annunLOW_PRESS_elec[0]) {
		ngxData.HYD_annunLOW_PRESS_elec[0] = data.HYD_annunLOW_PRESS_elec[0];
		js["HYD_annunLOW_PRESS_elec_1"] = data.HYD_annunLOW_PRESS_elec[0] != 0;
	}
	if (ngxData.HYD_annunLOW_PRESS_elec[1] != data.HYD_annunLOW_PRESS_elec[1]) {
		ngxData.HYD_annunLOW_PRESS_elec[1] = data.HYD_annunLOW_PRESS_elec[1];
		js["HYD_annunLOW_PRESS_elec_2"] = data.HYD_annunLOW_PRESS_elec[1] != 0;
	}
	if (ngxData.HYD_annunOVERHEAT_elec[0] != data.HYD_annunOVERHEAT_elec[0]) {
		ngxData.HYD_annunOVERHEAT_elec[0] = data.HYD_annunOVERHEAT_elec[0];
		js["HYD_annunOVERHEAT_elec_1"] = data.HYD_annunOVERHEAT_elec[0] != 0;
	}
	if (ngxData.HYD_annunOVERHEAT_elec[1] != data.HYD_annunOVERHEAT_elec[1]) {
		ngxData.HYD_annunOVERHEAT_elec[1] = data.HYD_annunOVERHEAT_elec[1];
		js["HYD_annunOVERHEAT_elec_2"] = data.HYD_annunOVERHEAT_elec[1] != 0;
	}
	if (ngxData.AIR_annunZoneTemp[0] != data.AIR_annunZoneTemp[0]) {
		ngxData.AIR_annunZoneTemp[0] = data.AIR_annunZoneTemp[0];
		js["AIR_annunZoneTemp_1"] = data.AIR_annunZoneTemp[0] != 0;
	}
	if (ngxData.AIR_annunZoneTemp[1] != data.AIR_annunZoneTemp[1]) {
		ngxData.AIR_annunZoneTemp[1] = data.AIR_annunZoneTemp[1];
		js["AIR_annunZoneTemp_2"] = data.AIR_annunZoneTemp[1] != 0;
	}
	if (ngxData.AIR_annunZoneTemp[2] != data.AIR_annunZoneTemp[2]) {
		ngxData.AIR_annunZoneTemp[2] = data.AIR_annunZoneTemp[2];
		js["AIR_annunZoneTemp_3"] = data.AIR_annunZoneTemp[2] != 0;
	}
	if (ngxData.AIR_annunPackTripOff[0] != data.AIR_annunPackTripOff[0]) {
		ngxData.AIR_annunPackTripOff[0] = data.AIR_annunPackTripOff[0];
		js["AIR_annunPackTripOff_1"] = data.AIR_annunPackTripOff[0] != 0;
	}
	if (ngxData.AIR_annunPackTripOff[1] != data.AIR_annunPackTripOff[1]) {
		ngxData.AIR_annunPackTripOff[1] = data.AIR_annunPackTripOff[1];
		js["AIR_annunPackTripOff_2"] = data.AIR_annunPackTripOff[1] != 0;
	}
	if (ngxData.AIR_annunWingBodyOverheat[0] != data.AIR_annunWingBodyOverheat[0]) {
		ngxData.AIR_annunWingBodyOverheat[0] = data.AIR_annunWingBodyOverheat[0];
		js["AIR_annunWingBodyOverheat_1"] = data.AIR_annunWingBodyOverheat[0] != 0;
	}
	if (ngxData.AIR_annunWingBodyOverheat[1] != data.AIR_annunWingBodyOverheat[1]) {
		ngxData.AIR_annunWingBodyOverheat[1] = data.AIR_annunWingBodyOverheat[1];
		js["AIR_annunWingBodyOverheat_2"] = data.AIR_annunWingBodyOverheat[1] != 0;
	}
	if (ngxData.AIR_annunBleedTripOff[0] != data.AIR_annunBleedTripOff[0]) {
		ngxData.AIR_annunBleedTripOff[0] = data.AIR_annunBleedTripOff[0];
		js["AIR_annunBleedTripOff_1"] = data.AIR_annunBleedTripOff[0] != 0;
	}
	if (ngxData.AIR_annunBleedTripOff[1] != data.AIR_annunBleedTripOff[1]) {
		ngxData.AIR_annunBleedTripOff[1] = data.AIR_annunBleedTripOff[1];
		js["AIR_annunBleedTripOff_2"] = data.AIR_annunBleedTripOff[1] != 0;
	}
	//if (ngxData.WARN_annunFIRE_WARN[0] != data.WARN_annunFIRE_WARN[0]) {
	//	ngxData.WARN_annunFIRE_WARN[0] = data.WARN_annunFIRE_WARN[0];
	//	js["WARN_annunFIRE_WARN_1"] = data.WARN_annunFIRE_WARN[0] != 0;
	//}
	//if (ngxData.WARN_annunFIRE_WARN[1] != data.WARN_annunFIRE_WARN[1]) {
	//	ngxData.WARN_annunFIRE_WARN[1] = data.WARN_annunFIRE_WARN[1];
	//	js["WARN_annunFIRE_WARN_2"] = data.WARN_annunFIRE_WARN[1] != 0;
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[0] != data.WARN_annunMASTER_CAUTION[0]) {
	//	ngxData.WARN_annunMASTER_CAUTION[0] = data.WARN_annunMASTER_CAUTION[0];
	//	js["WARN_annunMASTER_CAUTION_1"] = data.WARN_annunMASTER_CAUTION[0] != 0;
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[1] != data.WARN_annunMASTER_CAUTION[1]) {
	//	ngxData.WARN_annunMASTER_CAUTION[1] = data.WARN_annunMASTER_CAUTION[1];
	//	js["WARN_annunMASTER_CAUTION_2"] = data.WARN_annunMASTER_CAUTION[1] != 0;
	//}
	//if (ngxData.MAIN_annunBELOW_GS[0] != data.MAIN_annunBELOW_GS[0]) {
	//	ngxData.MAIN_annunBELOW_GS[0] = data.MAIN_annunBELOW_GS[0];
	//	js["MAIN_annunBELOW_GS_1"] = data.MAIN_annunBELOW_GS[0] != 0;
	//}
	//if (ngxData.MAIN_annunBELOW_GS[1] != data.MAIN_annunBELOW_GS[1]) {
	//	ngxData.MAIN_annunBELOW_GS[1] = data.MAIN_annunBELOW_GS[1];
	//	js["MAIN_annunBELOW_GS_2"] = data.MAIN_annunBELOW_GS[1] != 0;
	//}
	//if (ngxData.MAIN_annunAP[0] != data.MAIN_annunAP[0]) {
	//	ngxData.MAIN_annunAP[0] = data.MAIN_annunAP[0];
	//	js["MAIN_annunAP_1"] = data.MAIN_annunAP[0] != 0;
	//}
	//if (ngxData.MAIN_annunAP[1] != data.MAIN_annunAP[1]) {
	//	ngxData.MAIN_annunAP[1] = data.MAIN_annunAP[1];
	//	js["MAIN_annunAP_2"] = data.MAIN_annunAP[1] != 0;
	//}
	//if (ngxData.MAIN_annunAT[0] != data.MAIN_annunAT[0]) {
	//	ngxData.MAIN_annunAT[0] = data.MAIN_annunAT[0];
	//	js["MAIN_annunAT_1"] = data.MAIN_annunAT[0] != 0;
	//}
	//if (ngxData.MAIN_annunAT[1] != data.MAIN_annunAT[1]) {
	//	ngxData.MAIN_annunAT[1] = data.MAIN_annunAT[1];
	//	js["MAIN_annunAT_2"] = data.MAIN_annunAT[1] != 0;
	//}
	//if (ngxData.MAIN_annunFMC[0] != data.MAIN_annunFMC[0]) {
	//	ngxData.MAIN_annunFMC[0] = data.MAIN_annunFMC[0];
	//	js["MAIN_annunFMC_1"] = data.MAIN_annunFMC[0] != 0;
	//}
	//if (ngxData.MAIN_annunFMC[1] != data.MAIN_annunFMC[1]) {
	//	ngxData.MAIN_annunFMC[1] = data.MAIN_annunFMC[1];
	//	js["MAIN_annunFMC_2"] = data.MAIN_annunFMC[1] != 0;
	//}
	//if (ngxData.MAIN_annunGEAR_transit[0] != data.MAIN_annunGEAR_transit[0]) {
	//	ngxData.MAIN_annunGEAR_transit[0] = data.MAIN_annunGEAR_transit[0];
	//	js["MAIN_annunGEAR_transit_1"] = data.MAIN_annunGEAR_transit[0] != 0;
	//}
	//if (ngxData.MAIN_annunGEAR_transit[1] != data.MAIN_annunGEAR_transit[1]) {
	//	ngxData.MAIN_annunGEAR_transit[1] = data.MAIN_annunGEAR_transit[1];
	//	js["MAIN_annunGEAR_transit_2"] = data.MAIN_annunGEAR_transit[1] != 0;
	//}
	//if (ngxData.MAIN_annunGEAR_transit[2] != data.MAIN_annunGEAR_transit[2]) {
	//	ngxData.MAIN_annunGEAR_transit[2] = data.MAIN_annunGEAR_transit[2];
	//	js["MAIN_annunGEAR_transit_3"] = data.MAIN_annunGEAR_transit[2] != 0;
	//}

	if (ngxData.MAIN_annunGEAR_locked[0] != data.MAIN_annunGEAR_locked[0]) {
		ngxData.MAIN_annunGEAR_locked[0] = data.MAIN_annunGEAR_locked[0];
		js["MAIN_annunGEAR_locked_1"] = data.MAIN_annunGEAR_locked[0] != 0;
	}
	if (ngxData.MAIN_annunGEAR_locked[1] != data.MAIN_annunGEAR_locked[1]) {
		ngxData.MAIN_annunGEAR_locked[1] = data.MAIN_annunGEAR_locked[1];
		js["MAIN_annunGEAR_locked_2"] = data.MAIN_annunGEAR_locked[1] != 0;
	}
	if (ngxData.MAIN_annunGEAR_locked[2] != data.MAIN_annunGEAR_locked[2]) {
		ngxData.MAIN_annunGEAR_locked[2] = data.MAIN_annunGEAR_locked[2];
		js["MAIN_annunGEAR_locked_3"] = data.MAIN_annunGEAR_locked[2] != 0;
	}

	//if (ngxData.FIRE_annunENG_OVERHEAT[0] != data.FIRE_annunENG_OVERHEAT[0]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[0] = data.FIRE_annunENG_OVERHEAT[0];
	//	js["FIRE_annunENG_OVERHEAT_1"] = data.FIRE_annunENG_OVERHEAT[0] != 0;
	//}
	//if (ngxData.FIRE_annunENG_OVERHEAT[1] != data.FIRE_annunENG_OVERHEAT[1]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[1] = data.FIRE_annunENG_OVERHEAT[1];
	//	js["FIRE_annunENG_OVERHEAT_2"] = data.FIRE_annunENG_OVERHEAT[1] != 0;
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[0] != data.FIRE_annunBOTTLE_DISCHARGE[0]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[0] = data.FIRE_annunBOTTLE_DISCHARGE[0];
	//	js["FIRE_annunBOTTLE_DISCHARGE_1"] = data.FIRE_annunBOTTLE_DISCHARGE[0] != 0;
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[1] != data.FIRE_annunBOTTLE_DISCHARGE[1]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[1] = data.FIRE_annunBOTTLE_DISCHARGE[1];
	//	js["FIRE_annunBOTTLE_DISCHARGE_2"] = data.FIRE_annunBOTTLE_DISCHARGE[1] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[0] != data.FIRE_annunExtinguisherTest[0]) {
	//	ngxData.FIRE_annunExtinguisherTest[0] = data.FIRE_annunExtinguisherTest[0];
	//	js["FIRE_annunExtinguisherTest_1"] = data.FIRE_annunExtinguisherTest[0] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[1] != data.FIRE_annunExtinguisherTest[1]) {
	//	ngxData.FIRE_annunExtinguisherTest[1] = data.FIRE_annunExtinguisherTest[1];
	//	js["FIRE_annunExtinguisherTest_2"] = data.FIRE_annunExtinguisherTest[1] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[2] != data.FIRE_annunExtinguisherTest[2]) {
	//	ngxData.FIRE_annunExtinguisherTest[2] = data.FIRE_annunExtinguisherTest[2];
	//	js["FIRE_annunExtinguisherTest_3"] = data.FIRE_annunExtinguisherTest[2] != 0;
	//}
	//if (ngxData.CARGO_annunExtTest[0] != data.CARGO_annunExtTest[0]) {
	//	ngxData.CARGO_annunExtTest[0] = data.CARGO_annunExtTest[0];
	//	js["CARGO_annunExtTest_1"] = data.CARGO_annunExtTest[0] != 0;
	//}
	//if (ngxData.CARGO_annunExtTest[1] != data.CARGO_annunExtTest[1]) {
	//	ngxData.CARGO_annunExtTest[1] = data.CARGO_annunExtTest[1];
	//	js["CARGO_annunExtTest_2"] = data.CARGO_annunExtTest[1] != 0;
	//}
	//if (ngxData.MAIN_annunAP_Amber[0] != data.MAIN_annunAP_Amber[0]) {
	//	ngxData.MAIN_annunAP_Amber[0] = data.MAIN_annunAP_Amber[0];
	//	js["MAIN_annunAP_Amber_1"] = data.MAIN_annunAP_Amber[0] != 0;
	//}
	//if (ngxData.MAIN_annunAP_Amber[1] != data.MAIN_annunAP_Amber[1]) {
	//	ngxData.MAIN_annunAP_Amber[1] = data.MAIN_annunAP_Amber[1];
	//	js["MAIN_annunAP_Amber_2"] = data.MAIN_annunAP_Amber[1] != 0;
	//}
	//if (ngxData.MAIN_annunAT_Amber[0] != data.MAIN_annunAT_Amber[0]) {
	//	ngxData.MAIN_annunAT_Amber[0] = data.MAIN_annunAT_Amber[0];
	//	js["MAIN_annunAT_Amber_1"] = data.MAIN_annunAT_Amber[0] != 0;
	//}
	//if (ngxData.MAIN_annunAT_Amber[1] != data.MAIN_annunAT_Amber[1]) {
	//	ngxData.MAIN_annunAT_Amber[1] = data.MAIN_annunAT_Amber[1];
	//	js["MAIN_annunAT_Amber_2"] = data.MAIN_annunAT_Amber[1] != 0;
	//}

	return js.dump();

}