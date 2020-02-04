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
				LOG() << json_string;
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
				LOG() << json_string;
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


std::string Ifly737::initialize()
{
	bool valid = false;


	if (shareMemSDK->IFLY737NG_STATE == IFLY737NG_RUNNING)
	{
		valid = true;
	}
	json js;
	js["DataIsValid"] = valid;
	js["OXY_Needle"] = shareMemSDK->Oxygen_Pressure;

	js["IRS_DisplaySelector"] = shareMemSDK->DSPL_SEL_Switches_Status;
	js["IRS_SysDisplay_R"] = shareMemSDK->SYS_DSPL_Switches_Status;
	//	js["COMM_ServiceInterphoneSw"] = shareMemSDK->COMM_ServiceInterphoneSw != 0;
	js["OXY_SwNormal"] = shareMemSDK->Oxygen_Switch_Status == 0;

	//	js["FLTREC_SwNormal"] = !shareMemSDK->FLTREC_SwNormal != 0;

	js["FCTL_YawDamper_Sw"] = shareMemSDK->Yaw_Damper_Switches_Status != 0;
	js["FCTL_AltnFlaps_Control_Sw"] = shareMemSDK->Altn_Flap_Switches_Status;
	js["FCTL_AltnFlaps_Sw_ARM"] = shareMemSDK->Altn_Flap_Master_Switches_Status != 0;
	js["NAVDIS_VHFNavSelector"] = shareMemSDK->VHF_NAV_Switches_Status;
	js["NAVDIS_IRSSelector"] = shareMemSDK->IRS_Switches_Status;
	js["NAVDIS_FMCSelector"] = shareMemSDK->FMC_Switches_Status;
	js["NAVDIS_SourceSelector"] = shareMemSDK->Source_Switches_Status;
	js["NAVDIS_ControlPaneSelector"] = shareMemSDK->Control_Panel_Switches_Status;
	js["FUEL_CrossFeedSw"] = shareMemSDK->Fuel_Crossfeed_Selector_Status != 0;
	js["ELEC_DCMeterSelector"] = shareMemSDK->DC_Meters_Selector_Status;
	js["ELEC_ACMeterSelector"] = shareMemSDK->AC_Meters_Selector_Status;
	js["ELEC_BatSelector"] = shareMemSDK->Battery_Switch_Status == 0 ? 1 : 0;
	js["ELEC_CabUtilSw"] = shareMemSDK->CAB_UTIL_Switch_Status != 0;
	js["ELEC_IFEPassSeatSw"] = shareMemSDK->IFE_PASS_SEAT_Switches_Status != 0;
	js["ELEC_StandbyPowerSelector"] = shareMemSDK->STANDBY_POWER_Switch_Status;
	js["ELEC_GrdPwrSw"] = shareMemSDK->Ground_Power_Switches_Status;
	js["ELEC_BusTransSw_AUTO"] = shareMemSDK->Bus_Transfer_Switches_Status != 0;
	js["OH_WiperLSelector"] = shareMemSDK->Wiper_L_Switches_Status;
	js["OH_WiperRSelector"] = shareMemSDK->Wiper_R_Switches_Status;
	//	js["LTS_CircuitBreakerKnob"] = shareMemSDK->LTS_CircuitBreakerKnob;
	//	js["LTS_CircuitBreakerKnob"] = shareMemSDK->LTS_CircuitBreakerKnob;
	//	js["LTS_OvereadPanelKnob"] = shareMemSDK->LTS_OvereadPanelKnob;
	js["AIR_EquipCoolingSupplyNORM"] = shareMemSDK->Equipment_COOLING_SUPPLY_Switch_Status != 0;
	js["AIR_EquipCoolingExhaustNORM"] = shareMemSDK->Equipment_COOLING_EXHAUST_Switch_Status != 0;
	//	js["LTS_EmerExitSelector"] = shareMemSDK->Emergency_Light_Switch_Status;
	//	js["COMM_NoSmokingSelector"] = shareMemSDK->COMM_NoSmokingSelector;
	js["COMM_FastenBeltsSelector"] = shareMemSDK->Fasten_Belts_Switches_Status;
	js["ICE_WingAntiIceSw"] = shareMemSDK->Wing_AntiIce_Switch_Status != 0;
	//	js["AIR_TempSourceSelector"] = shareMemSDK->AIR_TempSourceSelector;
	js["AIR_TrimAirSwitch"] = shareMemSDK->TRIM_AIR_Switch_Status != 0;
	js["AIR_APUBleedAirSwitch"] = shareMemSDK->APU_Bleed_Air_Switches_Status != 0;
	js["AIR_IsolationValveSwitch"] = shareMemSDK->Isolation_Valve_Switches_Status;
	//	js["AIR_OutflowValveSwitch"] = shareMemSDK->AIR_OutflowValveSwitch != 0;
	js["AIR_PressurizationModeSelector"] = shareMemSDK->Pressurization_Mode_Selector_Status;
	js["LTS_TaxiSw"] = shareMemSDK->Taxi_Light_Status != 0;
	js["APU_Selector"] = shareMemSDK->APU_Switches_Status;
	js["ENG_IgnitionSelector"] = shareMemSDK->Ignition_Select_Switches_Status;
	//	js["LTS_LogoSw"] = shareMemSDK->LTS_LogoSw != 0;
	js["LTS_PositionSw"] = shareMemSDK->Position_Light_Status;
	js["LTS_AntiCollisionSw"] = shareMemSDK->Anti_Collision_Light_Status != 0;
	js["LTS_WingSw"] = shareMemSDK->Wing_Light_Status != 0;
	js["LTS_WheelWellSw"] = shareMemSDK->WHEEL_WELL_Light_Status != 0;
	//	js["MAIN_NoseWheelSteeringSwNORM"] = shareMemSDK->MAIN_NoseWheelSteeringSwNORM != 0;
	//	js["MAIN_LightsSelector"] = shareMemSDK->MAIN_LightsSelector != 0;
	//	js["MAIN_RMISelector1_VOR"] = shareMemSDK->MAIN_RMISelector1_VOR;
	//	js["MAIN_RMISelector2_VOR"] = shareMemSDK->MAIN_RMISelector2_VOR;
	js["MAIN_N1SetSelector"] = shareMemSDK->N1_Set_Status;
	js["MAIN_SpdRefSelector"] = shareMemSDK->N1_Set_Status;
	js["MAIN_FuelFlowSelector"] = shareMemSDK->Fuel_Flow_Status;
	js["MAIN_AutobrakeSelector"] = shareMemSDK->Autobrake_Switches_Status;
	js["MAIN_GearLever"] = shareMemSDK->Gear_Lever_Status;
	js["GPWS_FlapInhibitSw_NORM"] = shareMemSDK->Flap_Inhibit_Switches_Status != 0;
	js["GPWS_GearInhibitSw_NORM"] = shareMemSDK->Gear_Inhibit_Switches_Status != 0;
	js["GPWS_TerrInhibitSw_NORM"] = shareMemSDK->Terr_Inhibit_Switches_Status != 0;
	js["TRIM_StabTrimMainElecSw_NORMAL"] = shareMemSDK->Stab_Trim_Main_Elect_Switch_Status != 0;
	//	js["TRIM_StabTrimAutoPilotSw_NORMAL"] = shareMemSDK->Stab_Trim_Auto_Pilot_Switch_Status != 0;
	//	js["FIRE_DetTestSw"] = shareMemSDK->FIRE_DetTestSw != 0;
	//	js["FIRE_ExtinguisherTestSw"] = shareMemSDK->FIRE_ExtinguisherTestSw != 0;
	//	js["XPDR_XpndrSelector_2"] = shareMemSDK->XPDR_XpndrSelector_2;
	//	js["XPDR_AltSourceSel_2"] = shareMemSDK->XPDR_AltSourceSel_2;
	//	js["XPDR_ModeSel"] = shareMemSDK->XPDR_ModeSel;
	js["TRIM_StabTrimSw_NORMAL"] = shareMemSDK->Stabilizer_Trim_Switch_Status != 0;
	//	js["PED_FltDkDoorSel"] = shareMemSDK->PED_FltDkDoorSel != 0;
	//	js["IRS_aligned"] = shareMemSDK->IRS_aligned != 0;
	//	js["AircraftModel"] = shareMemSDK->AircraftModel;
	//	js["WeightInKg"] = shareMemSDK->WeightInKg;
	//	js["GPWS_V1CallEnabled"] = shareMemSDK->GPWS_V1CallEnabled != 0;
	//	js["GroundConnAvailable"] = shareMemSDK->GroundConnAvailable != 0;
	//	js["FMC_TakeoffFlaps"] = shareMemSDK->FMC_TakeoffFlaps;
	//	js["FMC_V1"] = shareMemSDK->FMC_V1;
	//	js["FMC_VR"] = shareMemSDK->FMC_VR;
	//	js["FMC_V2"] = shareMemSDK->FMC_V2;
	//	js["FMC_LandingFlaps"] = shareMemSDK->FMC_LandingFlaps;
	//	js["FMC_LandingVREF"] = shareMemSDK->FMC_LandingVREF;
	//	js["FMC_CruiseAlt"] = shareMemSDK->FMC_CruiseAlt;
	//	js["FMC_LandingAltitude"] = shareMemSDK->FMC_LandingAltitude;
	//	js["FMC_TransitionAlt"] = shareMemSDK->FMC_TransitionAlt;
	//	js["FMC_TransitionLevel"] = shareMemSDK->FMC_TransitionLevel;
	//	js["FMC_PerfInputComplete"] = shareMemSDK->FMC_PerfInputComplete != 0;
	//	js["FMC_DistanceToTOD"] = shareMemSDK->FMC_DistanceToTOD;
	//	js["FMC_DistanceToDest"] = shareMemSDK->FMC_DistanceToDest;
	//	js["ICE_WindowHeatTestSw"] = shareMemSDK->ICE_WindowHeatTestSw;
	js["IRS_ModeSelector_1"] = shareMemSDK->IRS_1_Switches_Status;
	js["IRS_ModeSelector_2"] = shareMemSDK->IRS_2_Switches_Status;
	js["ENG_EECSwitch_1"] = shareMemSDK->EEC_1_Switch_Status == 2;
	js["ENG_EECSwitch_2"] = shareMemSDK->EEC_2_Switch_Status == 2;
	js["FCTL_FltControl_Sw_1"] = shareMemSDK->Flight_Control_A_Switches_Status;
	js["FCTL_FltControl_Sw_2"] = shareMemSDK->Flight_Control_B_Switches_Status;
	js["FCTL_Spoiler_Sw_1"] = shareMemSDK->Spoiler_A_Switches_Status != 0;
	js["FCTL_Spoiler_Sw_2"] = shareMemSDK->Spoiler_B_Switches_Status != 0;
	js["FUEL_PumpFwdSw_1"] = shareMemSDK->Fuel_L_FWD_Switch_Status != 0;
	js["FUEL_PumpFwdSw_2"] = shareMemSDK->Fuel_R_FWD_Switch_Status != 0;
	js["FUEL_PumpAftSw_1"] = shareMemSDK->Fuel_L_AFT_Switch_Status != 0;
	js["FUEL_PumpAftSw_2"] = shareMemSDK->Fuel_R_AFT_Switch_Status != 0;
	js["FUEL_PumpCtrSw_1"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	js["FUEL_PumpCtrSw_2"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	js["ELEC_IDGDisconnectSw_1"] = shareMemSDK->Generator_1_Drive_Disconnect_Switch_Status != 0;
	js["ELEC_IDGDisconnectSw_2"] = shareMemSDK->Generator_2_Drive_Disconnect_Switch_Status != 0;
	//	js["ELEC_GenSw_1"] = shareMemSDK->ELEC_GenSw[0] != 0;
	//	js["ELEC_GenSw_2"] = shareMemSDK->ELEC_GenSw[1] != 0;
	js["ELEC_APUGenSw_1"] = shareMemSDK->APU_Generator_1_Switches_Status != 0;
	js["ELEC_APUGenSw_2"] = shareMemSDK->APU_Generator_2_Switches_Status != 0;
	js["ICE_WindowHeatSw_1"] = shareMemSDK->Window_Heat_Switch_1_Status != 0;
	js["ICE_WindowHeatSw_2"] = shareMemSDK->Window_Heat_Switch_2_Status != 0;
	js["ICE_WindowHeatSw_3"] = shareMemSDK->Window_Heat_Switch_3_Status != 0;
	js["ICE_WindowHeatSw_4"] = shareMemSDK->Window_Heat_Switch_4_Status != 0;
	js["ICE_ProbeHeatSw_1"] = shareMemSDK->Probe_Heat_Switch_1_Status != 0;
	js["ICE_ProbeHeatSw_2"] = shareMemSDK->Probe_Heat_Switch_2_Status != 0;
	js["ICE_EngAntiIceSw_1"] = shareMemSDK->Eng_1_AntiIce_Switch_Status != 0;
	js["ICE_EngAntiIceSw_2"] = shareMemSDK->Eng_2_AntiIce_Switch_Status != 0;
	js["HYD_PumpSw_eng_1"] = shareMemSDK->ENG_1_HYD_Switches_Status != 0;
	js["HYD_PumpSw_eng_2"] = shareMemSDK->ENG_2_HYD_Switches_Status != 0;
	js["HYD_PumpSw_elec_1"] = shareMemSDK->ELEC_1_HYD_Switches_Status != 0;
	js["HYD_PumpSw_elec_2"] = shareMemSDK->ELEC_2_HYD_Switches_Status != 0;
	js["AIR_RecircFanSwitch_1"] = shareMemSDK->RecircFan_L_Switch_Status != 0;
	js["AIR_RecircFanSwitch_2"] = shareMemSDK->RecircFan_R_Switch_Status != 0;
	js["AIR_PackSwitch_1"] = shareMemSDK->Pack_1_Switches_Status;
	js["AIR_PackSwitch_2"] = shareMemSDK->Pack_2_Switches_Status;
	js["AIR_BleedAirSwitch_1"] = shareMemSDK->Engine_1_Bleed_Air_Switches_Status != 0;
	js["AIR_BleedAirSwitch_2"] = shareMemSDK->Engine_2_Bleed_Air_Switches_Status != 0;
	js["LTS_LandingLtRetractableSw_1"] = shareMemSDK->Landing_Light_1_Status;
	js["LTS_LandingLtRetractableSw_2"] = shareMemSDK->Landing_Light_2_Status;
	js["LTS_LandingLtFixedSw_1"] = shareMemSDK->Landing_Light_3_Status != 0;
	js["LTS_LandingLtFixedSw_2"] = shareMemSDK->Landing_Light_4_Status != 0;
	js["LTS_RunwayTurnoffSw_1"] = shareMemSDK->Runway_Turnoff_Light_Status != 0;
	js["LTS_RunwayTurnoffSw_2"] = shareMemSDK->Runway_Turnoff_Light_Status != 0;
	js["ENG_StartSelector_1"] = shareMemSDK->Engine_1_Start_Switch_Status;
	js["ENG_StartSelector_2"] = shareMemSDK->Engine_2_Start_Switch_Status;
	//	js["EFIS_MinsSelBARO_1"] = shareMemSDK->EFIS_MinsSelBARO[0];
	//	js["EFIS_MinsSelBARO_2"] = shareMemSDK->EFIS_MinsSelBARO[1];
	//	js["EFIS_BaroSelHPA_1"] = shareMemSDK->EFIS_BaroSelHPA[0] != 0;
	//	js["EFIS_BaroSelHPA_2"] = shareMemSDK->EFIS_BaroSelHPA[1] != 0;
	js["MAIN_MainPanelDUSel_1"] = shareMemSDK->Main_Panel_DU_CAPT_Switches_Status;
	js["MAIN_MainPanelDUSel_2"] = shareMemSDK->Main_Panel_DU_FO_Switches_Status;
	//	js["MAIN_LowerDUSel_1"] = Lower_DU_Switches_Status;
	//	js["MAIN_LowerDUSel_2"] = Lower_DU_Switches_Status;
	//	js["MAIN_DisengageTestSelector_1"] = shareMemSDK->MAIN_DisengageTestSelector[0];
	//	js["MAIN_DisengageTestSelector_2"] = shareMemSDK->MAIN_DisengageTestSelector[1];
	js["FIRE_OvhtDetSw_1"] = shareMemSDK->OVHT_DET_1_Switches_Status;
	js["FIRE_OvhtDetSw_2"] = shareMemSDK->OVHT_DET_2_Switches_Status;
	//	js["FIRE_HandlePos_1"] = shareMemSDK->FIRE_HandlePos[0];
	//	js["FIRE_HandlePos_2"] = shareMemSDK->FIRE_HandlePos[1];
	//	js["FIRE_HandlePos_3"] = shareMemSDK->FIRE_HandlePos[2];
	//	js["FIRE_HandleIlluminated_1"] = shareMemSDK->FIRE_HandleIlluminated[0] != 0;
	//	js["FIRE_HandleIlluminated_2"] = shareMemSDK->FIRE_HandleIlluminated[1] != 0;
	//	js["FIRE_HandleIlluminated_3"] = shareMemSDK->FIRE_HandleIlluminated[2] != 0;
	js["CARGO_DetSelect_1"] = shareMemSDK->DET_Select_1_Switch_Status;
	js["CARGO_DetSelect_2"] = shareMemSDK->DET_Select_2_Switch_Status;
	//	js["CARGO_ArmedSw_1"] = shareMemSDK->CARGO_ArmedSw[0] != 0;
	//	js["CARGO_ArmedSw_2"] = shareMemSDK->CARGO_ArmedSw[1] != 0;
	js["ENG_StartValve_1"] = shareMemSDK->Engine_1_Start_Valve_Pos != 0;
	js["ENG_StartValve_2"] = shareMemSDK->Engine_2_Start_Valve_Pos != 0;
	//	js["COMM_SelectedMic_1"] = shareMemSDK->COMM_SelectedMic[0];
	//	js["COMM_SelectedMic_2"] = shareMemSDK->COMM_SelectedMic[1];
	//	js["COMM_SelectedMic_3"] = shareMemSDK->COMM_SelectedMic[2];
	//	js["COMM_ReceiverSwitches_1"] = shareMemSDK->COMM_ReceiverSwitches[0];
	//	js["COMM_ReceiverSwitches_2"] = shareMemSDK->COMM_ReceiverSwitches[1];
	//	js["COMM_ReceiverSwitches_3"] = shareMemSDK->COMM_ReceiverSwitches[2];
	js["IRS_annunGPS"] = shareMemSDK->GPS_Light_Status != 0;
	js["WARN_annunPSEU"] = shareMemSDK->PSEU_Light_Status != 0;
	js["OXY_annunPASS_OXY_ON"] = shareMemSDK->PASS_OXY_ON_Light_Status != 0;
	js["GEAR_annunOvhdLEFT"] = shareMemSDK->LEFT_GEAR_GreenLight_Status != 0;
	js["GEAR_annunOvhdNOSE"] = shareMemSDK->NOSE_GEAR_GreenLight_Status != 0;
	js["GEAR_annunOvhdRIGHT"] = shareMemSDK->RIGHT_GEAR_GreenLight_Status != 0;
	//	js["FLTREC_annunOFF"] = shareMemSDK->FLTREC_annunOFF != 0;
	js["FCTL_annunYAW_DAMPER"] = shareMemSDK->YAW_DAMPER_Light_Status != 0;
	js["FCTL_annunLOW_QUANTITY"] = shareMemSDK->LOW_QUANTITY_Light_Status != 0;
	js["FCTL_annunLOW_PRESSURE"] = shareMemSDK->LOW_PRESSURE_BACKUP_Light_Status != 0;
	js["FCTL_annunLOW_STBY_RUD_ON"] = shareMemSDK->STBY_RUD_ON_Light_Status != 0;
	js["FCTL_annunFEEL_DIFF_PRESS"] = shareMemSDK->FEEL_DIFF_PRESS_Light_Status != 0;
	js["FCTL_annunSPEED_TRIM_FAIL"] = shareMemSDK->SPEED_TRIM_FAIL_Light_Status != 0;
	js["FCTL_annunMACH_TRIM_FAIL"] = shareMemSDK->MACH_TRIM_FAIL_Light_Status != 0;
	js["FCTL_annunAUTO_SLAT_FAIL"] = shareMemSDK->AUTO_SLAT_FAIL_Light_Status != 0;
	js["FUEL_annunXFEED_VALVE_OPEN"] = shareMemSDK->VALVE_OPEN_Light_Status;
	js["ELEC_annunBAT_DISCHARGE"] = shareMemSDK->BAT_DISCHARGE_Light_Status != 0;
	js["ELEC_annunTR_UNIT"] = shareMemSDK->TR_UNIT_Light_Status != 0;
	js["ELEC_annunELEC"] = shareMemSDK->ELEC_Light_Status != 0;
	js["ELEC_annunSTANDBY_POWER_OFF"] = shareMemSDK->STANDBY_PWR_OFF_Light_Status != 0;
	js["ELEC_annunGRD_POWER_AVAILABLE"] = shareMemSDK->GRD_POWER_AVAILABLE_Light_Status != 0;
	js["ELEC_annunAPU_GEN_OFF_BUS"] = shareMemSDK->APU_GEN_OFF_BUS_Light_Status != 0;
	//	js["APU_annunMAINT"] = shareMemSDK->APU_annunMAINT != 0;
	//	js["APU_annunLOW_OIL_PRESSURE"] = shareMemSDK->APU_annunLOW_OIL_PRESSURE != 0;
	//	js["APU_annunFAULT"] = shareMemSDK->APU_annunFAULT != 0;
	//	js["APU_annunOVERSPEED"] = shareMemSDK->APU_annunOVERSPEED != 0;
	js["AIR_annunEquipCoolingSupplyOFF"] = shareMemSDK->Equip_Cooling_OFF_Light_1_Status != 0;
	js["AIR_annunEquipCoolingExhaustOFF"] = shareMemSDK->Equip_Cooling_OFF_Light_2_Status != 0;
	js["LTS_annunEmerNOT_ARMED"] = shareMemSDK->Emergency_Light_Status != 0;
	//	js["COMM_annunCALL"] = shareMemSDK->COMM_annunCALL != 0;
	//	js["COMM_annunPA_IN_USE"] = shareMemSDK->COMM_annunPA_IN_USE != 0;
	js["ICE_annunCAPT_PITOT"] = shareMemSDK->CAPT_PITOT_Lights_Status != 0;
	js["ICE_annunL_ELEV_PITOT"] = shareMemSDK->L_ELEV_PITOT_Lights_Status != 0;
	js["ICE_annunL_ALPHA_VANE"] = shareMemSDK->L_ALPHA_VANE_Lights_Status != 0;
	js["ICE_annunL_TEMP_PROBE"] = shareMemSDK->TEMP_PROBE_Lights_Status != 0;
	js["ICE_annunFO_PITOT"] = shareMemSDK->FO_PITOT_Lights_Status != 0;
	js["ICE_annunR_ELEV_PITOT"] = shareMemSDK->R_ELEV_PITOT_Lights_Status != 0;
	js["ICE_annunR_ALPHA_VANE"] = shareMemSDK->R_ALPHA_VANE_Lights_Status != 0;
	js["ICE_annunAUX_PITOT"] = shareMemSDK->AUX_PITOT_Lights_Status != 0;
	js["AIR_annunDualBleed"] = shareMemSDK->DUAL_BLEED_Light_Status != 0;
	js["AIR_annunRamDoorL"] = shareMemSDK->RAM_DOOR_FUEL_DOOR_1_Light_Status != 0;
	js["AIR_annunRamDoorR"] = shareMemSDK->RAM_DOOR_FUEL_DOOR_2_Light_Status != 0;
	//	js["AIR_FltAltWindow"] = shareMemSDK->AIR_FltAltWindow != 0;
	//	js["AIR_LandAltWindow"] = shareMemSDK->AIR_LandAltWindow != 0;
	js["WARN_annunFLT_CONT"] = shareMemSDK->Warning_FLT_CONT_Light_Status != 0;
	js["WARN_annunIRS"] = shareMemSDK->Warning_IRS_Light_Status != 0;
	js["WARN_annunFUEL"] = shareMemSDK->Warning_FUEL_Light_Status != 0;
	js["WARN_annunELEC"] = shareMemSDK->Warning_ELEC_Light_Status != 0;
	js["WARN_annunAPU"] = shareMemSDK->Warning_APU_Light_Status != 0;
	js["WARN_annunOVHT_DET"] = shareMemSDK->Warning_OVHT_Light_Status != 0;
	js["WARN_annunANTI_ICE"] = shareMemSDK->Warning_ANTIICE_Light_Status != 0;
	js["WARN_annunHYD"] = shareMemSDK->Warning_HYD_Light_Status != 0;
	js["WARN_annunDOORS"] = shareMemSDK->Warning_DOORS_Light_Status != 0;
	js["WARN_annunENG"] = shareMemSDK->Warning_ENG_Light_Status != 0;
	js["WARN_annunOVERHEAD"] = shareMemSDK->Warning_OVERHEAD_Light_Status != 0;
	js["WARN_annunAIR_COND"] = shareMemSDK->Warning_AIR_COND_Light_Status != 0;
	js["MAIN_annunSPEEDBRAKE_ARMED"] = shareMemSDK->SPEED_BRAKE_ARMED_Light_Status != 0;
	js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = shareMemSDK->SPEED_BRAKE_DO_NOT_ARM_Light_Status != 0;
	js["MAIN_annunSPEEDBRAKE_EXTENDED"] = shareMemSDK->SPEEDBRAKES_EXTENDED_Light_Status != 0;
	//	js["MAIN_annunSTAB_OUT_OF_TRIM"] = shareMemSDK->MAIN_annunSTAB_OUT_OF_TRIM != 0;
	js["MAIN_annunANTI_SKID_INOP"] = shareMemSDK->ANTISKID_INOP_Light_Status != 0;
	js["MAIN_annunAUTO_BRAKE_DISARM"] = shareMemSDK->AUTO_BRAKE_DISARM_Light_Status != 0;
	//	js["MAIN_annunLE_FLAPS_TRANSIT"] = shareMemSDK->MAIN_annunLE_FLAPS_TRANSIT != 0;
	//	js["MAIN_annunLE_FLAPS_EXT"] = shareMemSDK->MAIN_annunLE_FLAPS_EXT != 0;
	//	js["HGS_annun_AIII"] = shareMemSDK->HGS_annun_AIII != 0;
	//	js["HGS_annun_NO_AIII"] = shareMemSDK->HGS_annun_NO_AIII != 0;
	//	js["HGS_annun_FLARE"] = shareMemSDK->HGS_annun_FLARE != 0;
	//	js["HGS_annun_RO"] = shareMemSDK->HGS_annun_RO != 0;
	//	js["HGS_annun_RO_CTN"] = shareMemSDK->HGS_annun_RO_CTN != 0;
	//	js["HGS_annun_RO_ARM"] = shareMemSDK->HGS_annun_RO_ARM != 0;
	//	js["HGS_annun_TO"] = shareMemSDK->HGS_annun_TO != 0;
	//	js["HGS_annun_TO_CTN"] = shareMemSDK->HGS_annun_TO_CTN != 0;
	//	js["HGS_annun_APCH"] = shareMemSDK->HGS_annun_APCH != 0;
	//	js["HGS_annun_TO_WARN"] = shareMemSDK->HGS_annun_TO_WARN != 0;
	//	js["HGS_annun_Bar"] = shareMemSDK->HGS_annun_Bar != 0;
	//	js["HGS_annun_FAIL"] = shareMemSDK->HGS_annun_FAIL != 0;
	//	js["GPWS_annunINOP"] = shareMemSDK->GPWS_annunINOP != 0;
	js["PED_annunParkingBrake"] = shareMemSDK->Parking_Brake_Light_Status != 0;
	//	js["FIRE_annunWHEEL_WELL"] = shareMemSDK->FIRE_annunWHEEL_WELL != 0;
	//	js["FIRE_annunFAULT"] = shareMemSDK->FIRE_annunFAULT != 0;
	//	js["FIRE_annunAPU_DET_INOP"] = shareMemSDK->FIRE_annunAPU_DET_INOP != 0;
	//	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = shareMemSDK->FIRE_annunAPU_BOTTLE_DISCHARGE != 0;
	//	js["CARGO_annunFWD"] = shareMemSDK->CARGO_annunFWD != 0;
	//	js["CARGO_annunAFT"] = shareMemSDK->CARGO_annunAFT != 0;
	//	js["CARGO_annunDETECTOR_FAULT"] = shareMemSDK->CARGO_annunDETECTOR_FAULT != 0;
	//	js["CARGO_annunDISCH"] = shareMemSDK->CARGO_annunDISCH != 0;
		//js["HGS_annunRWY"] = shareMemSDK->HGS_annunRWY != 0;
		//js["HGS_annunGS"] = shareMemSDK->HGS_annunGS != 0;
		//js["HGS_annunFAULT"] = shareMemSDK->HGS_annunFAULT != 0;
		//js["HGS_annunCLR"] = shareMemSDK->HGS_annunCLR != 0;
		//js["XPDR_annunFAIL"] = shareMemSDK->XPDR_annunFAIL != 0;
		//js["PED_annunLOCK_FAIL"] = shareMemSDK->PED_annunLOCK_FAIL != 0;
		//js["PED_annunAUTO_UNLK"] = shareMemSDK->PED_annunAUTO_UNLK != 0;
		//js["DOOR_annunFWD_ENTRY"] = shareMemSDK->DOOR_annunFWD_ENTRY != 0;
		//js["DOOR_annunFWD_SERVICE"] = shareMemSDK->DOOR_annunFWD_SERVICE != 0;
		//js["DOOR_annunAIRSTAIR"] = shareMemSDK->DOOR_annunAIRSTAIR != 0;
		//js["DOOR_annunLEFT_FWD_OVERWING"] = shareMemSDK->DOOR_annunLEFT_FWD_OVERWING != 0;
		//js["DOOR_annunRIGHT_FWD_OVERWING"] = shareMemSDK->DOOR_annunRIGHT_FWD_OVERWING != 0;
		//js["DOOR_annunFWD_CARGO"] = shareMemSDK->DOOR_annunFWD_CARGO != 0;
		//js["DOOR_annunEQUIP"] = shareMemSDK->DOOR_annunEQUIP != 0;
		//js["DOOR_annunLEFT_AFT_OVERWING"] = shareMemSDK->DOOR_annunLEFT_AFT_OVERWING != 0;
		//js["DOOR_annunRIGHT_AFT_OVERWING"] = shareMemSDK->DOOR_annunRIGHT_AFT_OVERWING != 0;
		//js["DOOR_annunAFT_CARGO"] = shareMemSDK->DOOR_annunAFT_CARGO != 0;
		//js["DOOR_annunAFT_ENTRY"] = shareMemSDK->DOOR_annunAFT_ENTRY != 0;
		//js["DOOR_annunAFT_SERVICE"] = shareMemSDK->DOOR_annunAFT_SERVICE != 0;
	js["AIR_annunAUTO_FAIL"] = shareMemSDK->AUTO_FAIL_Light_Status != 0;
	js["AIR_annunOFFSCHED_DESCENT"] = shareMemSDK->OFF_SCHED_DESCENT_Light_Status != 0;
	js["AIR_annunALTN"] = shareMemSDK->ALTN_Light_Status != 0;
	js["AIR_annunMANUAL"] = shareMemSDK->MANUAL_Light_Status != 0;
	//js["IRS_annunALIGN_1"] = shareMemSDK->IRS_annunALIGN[0] != 0;
	//js["IRS_annunALIGN_2"] = shareMemSDK->IRS_annunALIGN[1] != 0;
	//js["IRS_annunON_DC_1"] = shareMemSDK->IRS_annunON_DC[0] != 0;
	//js["IRS_annunON_DC_2"] = shareMemSDK->IRS_annunON_DC[1] != 0;
	//js["IRS_annunFAULT_1"] = shareMemSDK->IRS_annunFAULT[0] != 0;
	//js["IRS_annunFAULT_2"] = shareMemSDK->IRS_annunFAULT[1] != 0;
	//js["IRS_annunDC_FAIL_1"] = shareMemSDK->IRS_annunDC_FAIL[0] != 0;
	//js["IRS_annunDC_FAIL_2"] = shareMemSDK->IRS_annunDC_FAIL[1] != 0;
	js["ENG_annunREVERSER_1"] = shareMemSDK->REVERSER_1_Light_Status != 0;
	js["ENG_annunREVERSER_2"] = shareMemSDK->REVERSER_2_Light_Status != 0;
	js["ENG_annunENGINE_CONTROL_1"] = shareMemSDK->ENGINE_CONTROL_1_Light_Status != 0;
	js["ENG_annunENGINE_CONTROL_2"] = shareMemSDK->ENGINE_CONTROL_2_Light_Status != 0;
	//	js["ENG_annunALTN_1"] = shareMemSDK->ENG_annunALTN[0] != 0;
	//	js["ENG_annunALTN_2"] = shareMemSDK->ENG_annunALTN[1] != 0;
	js["FCTL_annunFC_LOW_PRESSURE_1"] = shareMemSDK->LOW_PRESSURE_A_Light_Status != 0;
	js["FCTL_annunFC_LOW_PRESSURE_2"] = shareMemSDK->LOW_PRESSURE_B_Light_Status != 0;

	js["FUEL_annunENG_VALVE_CLOSED_1"] = shareMemSDK->ENG_VALVE_CLOSED_Light_1_Status != 0 != 0;
	js["FUEL_annunENG_VALVE_CLOSED_2"] = shareMemSDK->ENG_VALVE_CLOSED_Light_2_Status != 0 != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_1"] = shareMemSDK->SPAR_VALVE_CLOSED_Light_1_Status != 0 != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_2"] = shareMemSDK->SPAR_VALVE_CLOSED_Light_2_Status != 0 != 0;

	js["FUEL_annunFILTER_BYPASS_1"] = shareMemSDK->FILTER_BYPASS_Light_1_Status != 0;
	js["FUEL_annunFILTER_BYPASS_2"] = shareMemSDK->FILTER_BYPASS_Light_2_Status != 0;
	js["FUEL_annunLOWPRESS_Fwd_1"] = shareMemSDK->LOW_PRESSURE_Light_L_FWD_Status != 0;
	js["FUEL_annunLOWPRESS_Fwd_2"] = shareMemSDK->LOW_PRESSURE_Light_R_FWD_Status != 0;
	js["FUEL_annunLOWPRESS_Aft_1"] = shareMemSDK->LOW_PRESSURE_Light_L_AFT_Status != 0;
	js["FUEL_annunLOWPRESS_Aft_2"] = shareMemSDK->LOW_PRESSURE_Light_R_AFT_Status != 0;
	js["FUEL_annunLOWPRESS_Ctr_1"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	js["FUEL_annunLOWPRESS_Ctr_2"] = shareMemSDK->Fuel_CENTER_R_Switch_Status != 0;
	js["ELEC_annunDRIVE_1"] = shareMemSDK->ENG_1_DRIVE_Light_Status != 0;
	js["ELEC_annunDRIVE_2"] = shareMemSDK->ENG_2_DRIVE_Light_Status != 0;
	js["ELEC_annunTRANSFER_BUS_OFF_1"] = shareMemSDK->ENG_1_TRANSFER_BUS_OFF_Light_Status != 0;
	js["ELEC_annunTRANSFER_BUS_OFF_2"] = shareMemSDK->ENG_2_TRANSFER_BUS_OFF_Light_Status != 0;
	js["ELEC_annunSOURCE_OFF_1"] = shareMemSDK->ENG_1_SOURCE_OFF_Light_Status != 0;
	js["ELEC_annunSOURCE_OFF_2"] = shareMemSDK->ENG_2_SOURCE_OFF_Light_Status != 0;
	js["ELEC_annunGEN_BUS_OFF_1"] = shareMemSDK->ENG_1_GEN_OFF_BUS_Light_Status != 0;
	js["ELEC_annunGEN_BUS_OFF_2"] = shareMemSDK->ENG_2_GEN_OFF_BUS_Light_Status != 0;
	js["ICE_annunOVERHEAT_1"] = shareMemSDK->Window_OVERHEAT_Lights_1_Status != 0;
	js["ICE_annunOVERHEAT_2"] = shareMemSDK->Window_OVERHEAT_Lights_2_Status != 0;
	js["ICE_annunOVERHEAT_3"] = shareMemSDK->Window_OVERHEAT_Lights_3_Status != 0;
	js["ICE_annunOVERHEAT_4"] = shareMemSDK->Window_OVERHEAT_Lights_4_Status != 0;
	js["ICE_annunON_1"] = shareMemSDK->Window_Heat_ON_Lights_1_Status != 0;
	js["ICE_annunON_2"] = shareMemSDK->Window_Heat_ON_Lights_2_Status != 0;
	js["ICE_annunON_3"] = shareMemSDK->Window_Heat_ON_Lights_3_Status != 0;
	js["ICE_annunON_4"] = shareMemSDK->Window_Heat_ON_Lights_4_Status != 0;
	js["ICE_annunVALVE_OPEN_1"] = shareMemSDK->L_VALVE_OPEN_Lights_Status != 0;
	js["ICE_annunVALVE_OPEN_2"] = shareMemSDK->R_VALVE_OPEN_Lights_Status != 0;
	js["ICE_annunCOWL_ANTI_ICE_1"] = shareMemSDK->COWL_ANTI_ICE_Lights_1_Status != 0;
	js["ICE_annunCOWL_ANTI_ICE_2"] = shareMemSDK->COWL_ANTI_ICE_Lights_2_Status != 0;
	js["ICE_annunCOWL_VALVE_OPEN_1"] = shareMemSDK->COWL_VALVE_OPEN_Lights_1_Status != 0;
	js["ICE_annunCOWL_VALVE_OPEN_2"] = shareMemSDK->COWL_VALVE_OPEN_Lights_2_Status != 0;
	js["HYD_annunLOW_PRESS_eng_1"] = shareMemSDK->ENG_1_HYD_LOW_PRESSURE_Light_Status != 0;
	js["HYD_annunLOW_PRESS_eng_2"] = shareMemSDK->ENG_2_HYD_LOW_PRESSURE_Light_Status != 0;
	js["HYD_annunLOW_PRESS_elec_1"] = shareMemSDK->ELEC_1_HYD_LOW_PRESSURE_Light_Status != 0;
	js["HYD_annunLOW_PRESS_elec_2"] = shareMemSDK->ELEC_2_HYD_LOW_PRESSURE_Light_Status != 0;
	js["HYD_annunOVERHEAT_elec_1"] = shareMemSDK->ELEC_1_HYD_OVERHEAT_Light_Status != 0;
	js["HYD_annunOVERHEAT_elec_2"] = shareMemSDK->ELEC_2_HYD_OVERHEAT_Light_Status != 0;
	js["AIR_annunZoneTemp_1"] = shareMemSDK->ZONE_TEMP_Light_1_Status != 0;
	js["AIR_annunZoneTemp_2"] = shareMemSDK->ZONE_TEMP_Light_2_Status != 0;
	js["AIR_annunZoneTemp_3"] = shareMemSDK->ZONE_TEMP_Light_3_Status != 0;
	js["AIR_annunPackTripOff_1"] = shareMemSDK->PACK_TRIP_OFF_1_Light_Status != 0;
	js["AIR_annunPackTripOff_2"] = shareMemSDK->PACK_TRIP_OFF_2_Light_Status != 0;
	js["AIR_annunWingBodyOverheat_1"] = shareMemSDK->WING_BODY_OVERHEAT_1_Light_Status != 0;
	js["AIR_annunWingBodyOverheat_2"] = shareMemSDK->WING_BODY_OVERHEAT_2_Light_Status != 0;
	js["AIR_annunBleedTripOff_1"] = shareMemSDK->BLEED_TRIP_OFF_1_Light_Status != 0;
	js["AIR_annunBleedTripOff_2"] = shareMemSDK->BLEED_TRIP_OFF_2_Light_Status != 0;
	//js["WARN_annunFIRE_WARN_1"] = shareMemSDK->WARN_annunFIRE_WARN[0] != 0;
	//js["WARN_annunFIRE_WARN_2"] = shareMemSDK->WARN_annunFIRE_WARN[1] != 0;
	//js["WARN_annunMASTER_CAUTION_1"] = shareMemSDK->WARN_annunMASTER_CAUTION[0] != 0;
	//js["WARN_annunMASTER_CAUTION_2"] = shareMemSDK->WARN_annunMASTER_CAUTION[1] != 0;
	//js["MAIN_annunBELOW_GS_1"] = shareMemSDK->MAIN_annunBELOW_GS[0] != 0;
	//js["MAIN_annunBELOW_GS_2"] = shareMemSDK->MAIN_annunBELOW_GS[1] != 0;
	//js["MAIN_annunAP_1"] = shareMemSDK->MAIN_annunAP[0] != 0;
	//js["MAIN_annunAP_2"] = shareMemSDK->MAIN_annunAP[1] != 0;
	//js["MAIN_annunAT_1"] = shareMemSDK->MAIN_annunAT[0] != 0;
	//js["MAIN_annunAT_2"] = shareMemSDK->MAIN_annunAT[1] != 0;
	//js["MAIN_annunFMC_1"] = shareMemSDK->MAIN_annunFMC[0] != 0;
	//js["MAIN_annunFMC_2"] = shareMemSDK->MAIN_annunFMC[1] != 0;
	js["MAIN_annunGEAR_transit_1"] = shareMemSDK->LEFT_GEAR_RedLight_Status != 0;
	js["MAIN_annunGEAR_transit_2"] = shareMemSDK->NOSE_GEAR_RedLight_Status != 0;
	js["MAIN_annunGEAR_transit_3"] = shareMemSDK->RIGHT_GEAR_RedLight_Status != 0;
	js["MAIN_annunGEAR_locked_1"] = shareMemSDK->LEFT_GEAR_GreenLight_Status != 0;
	js["MAIN_annunGEAR_locked_2"] = shareMemSDK->NOSE_GEAR_GreenLight_Status != 0;
	js["MAIN_annunGEAR_locked_3"] = shareMemSDK->RIGHT_GEAR_GreenLight_Status != 0;
	//js["FIRE_annunENG_OVERHEAT_1"] = shareMemSDK->FIRE_annunENG_OVERHEAT[0] != 0;
	//js["FIRE_annunENG_OVERHEAT_2"] = shareMemSDK->FIRE_annunENG_OVERHEAT[1] != 0;
	//js["FIRE_annunBOTTLE_DISCHARGE_1"] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[0] != 0;
	//js["FIRE_annunBOTTLE_DISCHARGE_2"] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[1] != 0;
	//js["FIRE_annunExtinguisherTest_1"] = shareMemSDK->FIRE_annunExtinguisherTest[0] != 0;
	//js["FIRE_annunExtinguisherTest_2"] = shareMemSDK->FIRE_annunExtinguisherTest[1] != 0;
	//js["FIRE_annunExtinguisherTest_3"] = shareMemSDK->FIRE_annunExtinguisherTest[2] != 0;
	//js["CARGO_annunExtTest_1"] = shareMemSDK->CARGO_annunExtTest[0] != 0;
	//js["CARGO_annunExtTest_2"] = shareMemSDK->CARGO_annunExtTest[1] != 0;
	//js["MAIN_annunAP_Amber_1"] = shareMemSDK->MAIN_annunAP_Amber[0] != 0;
	//js["MAIN_annunAP_Amber_2"] = shareMemSDK->MAIN_annunAP_Amber[1] != 0;
	//js["MAIN_annunAT_Amber_1"] = shareMemSDK->MAIN_annunAT_Amber[0] != 0;
	//js["MAIN_annunAT_Amber_2"] = shareMemSDK->MAIN_annunAT_Amber[1] != 0;
	js["MCP_FDSw_1"] = shareMemSDK->FD_left_Switches_Status != 0;
	js["MCP_FDSw_2"] = shareMemSDK->FD_right_Switches_Status != 0;
	js["EFIS_ModeSel_1"] = shareMemSDK->FO_ND_Mode_status;
	js["EFIS_ModeSel_2"] = shareMemSDK->FO_ND_Mode_status;
	js["MCP_DisengageBar"] = shareMemSDK->DISENGAGE_Bar_Switches_Status != 0;

	return js.dump();
}




std::string Ifly737::buildJsonIfly737()
{

	json js;

	if (ngxData.DISENGAGE_Bar_Switches_Status != shareMemSDK->DISENGAGE_Bar_Switches_Status) {
		ngxData.DISENGAGE_Bar_Switches_Status = shareMemSDK->DISENGAGE_Bar_Switches_Status;
		js["MCP_DisengageBar"] = shareMemSDK->DISENGAGE_Bar_Switches_Status != 0;
	}
	if (ngxData.FO_ND_Mode_status != shareMemSDK->FO_ND_Mode_status)
	{
		ngxData.FO_ND_Mode_status = shareMemSDK->FO_ND_Mode_status;
		js["EFIS_ModeSel_1"] = shareMemSDK->FO_ND_Mode_status;
	}
	if (ngxData.FO_ND_Mode_status != shareMemSDK->FO_ND_Mode_status)
	{
		ngxData.FO_ND_Mode_status = shareMemSDK->FO_ND_Mode_status;
		js["EFIS_ModeSel_2"] = shareMemSDK->FO_ND_Mode_status != 0;
	}
	if (ngxData.FD_left_Switches_Status != shareMemSDK->FD_left_Switches_Status) {
		ngxData.FD_left_Switches_Status = shareMemSDK->FD_left_Switches_Status;
		js["MCP_FDSw_1"] = shareMemSDK->FD_left_Switches_Status != 0;
	}
	if (ngxData.FD_right_Switches_Status != shareMemSDK->FD_right_Switches_Status) {
		ngxData.FD_right_Switches_Status = shareMemSDK->FD_right_Switches_Status;
		js["MCP_FDSw_2"] = shareMemSDK->FD_right_Switches_Status != 0;
	}

	if (abs(ngxData.Oxygen_Pressure - shareMemSDK->Oxygen_Pressure) > 20)
	{
		ngxData.Oxygen_Pressure = shareMemSDK->Oxygen_Pressure;
		js["OXY_Needle"] = shareMemSDK->Oxygen_Pressure != 0;
	}
	//if (ngxData.IRS_DisplaySelector != shareMemSDK->IRS_DisplaySelector) {
	//	ngxData.IRS_DisplaySelector = shareMemSDK->IRS_DisplaySelector;
	//	js["IRS_DisplaySelector"] = shareMemSDK->IRS_DisplaySelector;
	//}

	//if (ngxData.IRS_SysDisplay_R != shareMemSDK->IRS_SysDisplay_R) {
	//	ngxData.IRS_SysDisplay_R = shareMemSDK->IRS_SysDisplay_R;
	//	js["IRS_SysDisplay_R"] = shareMemSDK->IRS_SysDisplay_R;
	//}

	//if (ngxData.COMM_ServiceInterphoneSw != shareMemSDK->COMM_ServiceInterphoneSw) {
	//	ngxData.COMM_ServiceInterphoneSw = shareMemSDK->COMM_ServiceInterphoneSw;
	//	js["COMM_ServiceInterphoneSw"] = shareMemSDK->COMM_ServiceInterphoneSw != 0;
	//}
	if (ngxData.Oxygen_Switch_Status != shareMemSDK->Oxygen_Switch_Status) {
		ngxData.Oxygen_Switch_Status = shareMemSDK->Oxygen_Switch_Status;
		js["OXY_SwNormal"] = shareMemSDK->Oxygen_Switch_Status == 0;
	}

	if (ngxData.Yaw_Damper_Switches_Status != shareMemSDK->Yaw_Damper_Switches_Status) {
		ngxData.Yaw_Damper_Switches_Status = shareMemSDK->Yaw_Damper_Switches_Status;
		js["FCTL_YawDamper_Sw"] = shareMemSDK->Yaw_Damper_Switches_Status != 0;
	}
	if (ngxData.Altn_Flap_Switches_Status != shareMemSDK->Altn_Flap_Switches_Status) {
		ngxData.Altn_Flap_Switches_Status = shareMemSDK->Altn_Flap_Switches_Status;
		js["FCTL_AltnFlaps_Control_Sw"] = shareMemSDK->Altn_Flap_Switches_Status;
	}
	if (ngxData.Altn_Flap_Master_Switches_Status != shareMemSDK->Altn_Flap_Master_Switches_Status) {
		ngxData.Altn_Flap_Master_Switches_Status = shareMemSDK->Altn_Flap_Master_Switches_Status;
		js["FCTL_AltnFlaps_Sw_ARM"]  = shareMemSDK->Altn_Flap_Master_Switches_Status != 0;
	}
	if (ngxData.VHF_NAV_Switches_Status != shareMemSDK->VHF_NAV_Switches_Status) {
		ngxData.VHF_NAV_Switches_Status = shareMemSDK->VHF_NAV_Switches_Status;
		js["NAVDIS_VHFNavSelector"] = shareMemSDK->VHF_NAV_Switches_Status;
	}
	if (ngxData.IRS_Switches_Status != shareMemSDK->IRS_Switches_Status) {
		ngxData.IRS_Switches_Status = shareMemSDK->IRS_Switches_Status;
		js["NAVDIS_IRSSelector"] = shareMemSDK->IRS_Switches_Status;
	}
	if (ngxData.FMC_Switches_Status != shareMemSDK->FMC_Switches_Status) {
		ngxData.FMC_Switches_Status = shareMemSDK->FMC_Switches_Status;
		js["NAVDIS_FMCSelector"] = shareMemSDK->FMC_Switches_Status;
	}
	if (ngxData.Source_Switches_Status != shareMemSDK->Source_Switches_Status) {
		ngxData.Source_Switches_Status = shareMemSDK->Source_Switches_Status;
		js["NAVDIS_SourceSelector"] = shareMemSDK->Source_Switches_Status;
	}
	if (ngxData.Control_Panel_Switches_Status != shareMemSDK->Control_Panel_Switches_Status) {
		ngxData.Control_Panel_Switches_Status = shareMemSDK->Control_Panel_Switches_Status;
		js["NAVDIS_ControlPaneSelector"] = shareMemSDK->Control_Panel_Switches_Status;
	}
	if (ngxData.Fuel_Crossfeed_Selector_Status != shareMemSDK->Fuel_Crossfeed_Selector_Status) {
		ngxData.Fuel_Crossfeed_Selector_Status = shareMemSDK->Fuel_Crossfeed_Selector_Status;
		js["FUEL_CrossFeedSw"] = shareMemSDK->Fuel_Crossfeed_Selector_Status != 0;
	}
	if (ngxData.DC_Meters_Selector_Status != shareMemSDK->DC_Meters_Selector_Status) {
		ngxData.DC_Meters_Selector_Status = shareMemSDK->DC_Meters_Selector_Status;
		js["ELEC_DCMeterSelector"] = shareMemSDK->DC_Meters_Selector_Status;
	}
	//if (ngxData.AC_Meters_Selector_Status != shareMemSDK->AC_Meters_Selector_Status) {
	//	ngxData.AC_Meters_Selector_Status = shareMemSDK->AC_Meters_Selector_Status;
	//	js["ELEC_ACMeterSelector"] = shareMemSDK->AC_Meters_Selector_Status;
	//}
	if (ngxData.Battery_Switch_Status != shareMemSDK->Battery_Switch_Status) {
		ngxData.Battery_Switch_Status = shareMemSDK->Battery_Switch_Status;
		js["ELEC_BatSelector"] = shareMemSDK->Battery_Switch_Status == 0? 1 : 0;
	}
	if (ngxData.CAB_UTIL_Switch_Status != shareMemSDK->CAB_UTIL_Switch_Status) {
		ngxData.CAB_UTIL_Switch_Status = shareMemSDK->CAB_UTIL_Switch_Status;
		js["ELEC_CabUtilSw"] = shareMemSDK->CAB_UTIL_Switch_Status != 0;
	}
	if (ngxData.IFE_PASS_SEAT_Switches_Status != shareMemSDK->IFE_PASS_SEAT_Switches_Status) {
		ngxData.IFE_PASS_SEAT_Switches_Status = shareMemSDK->IFE_PASS_SEAT_Switches_Status;
		js["ELEC_IFEPassSeatSw"] = shareMemSDK->IFE_PASS_SEAT_Switches_Status != 0;
	}
	if (ngxData.STANDBY_POWER_Switch_Status != shareMemSDK->STANDBY_POWER_Switch_Status) {
		ngxData.STANDBY_POWER_Switch_Status = shareMemSDK->STANDBY_POWER_Switch_Status;
		js["ELEC_StandbyPowerSelector"] = shareMemSDK->STANDBY_POWER_Switch_Status;
	}
	if (ngxData.Ground_Power_Switches_Status != shareMemSDK->Ground_Power_Switches_Status) {
		ngxData.Ground_Power_Switches_Status = shareMemSDK->Ground_Power_Switches_Status;
		js["ELEC_GrdPwrSw"] = shareMemSDK->Ground_Power_Switches_Status;
	}
	if (ngxData.Bus_Transfer_Switches_Status != shareMemSDK->Bus_Transfer_Switches_Status) {
		ngxData.Bus_Transfer_Switches_Status = shareMemSDK->Bus_Transfer_Switches_Status;
		js["ELEC_BusTransSw_AUTO"] = shareMemSDK->Bus_Transfer_Switches_Status != 0;
	}
	if (ngxData.Wiper_L_Switches_Status != shareMemSDK->Wiper_L_Switches_Status) {
		ngxData.Wiper_L_Switches_Status = shareMemSDK->Wiper_L_Switches_Status;
		js["OH_WiperLSelector"] = shareMemSDK->Wiper_L_Switches_Status;
	}
	if (ngxData.Wiper_R_Switches_Status != shareMemSDK->Wiper_R_Switches_Status) {
		ngxData.Wiper_R_Switches_Status = shareMemSDK->Wiper_R_Switches_Status;
		js["OH_WiperRSelector"] = shareMemSDK->Wiper_R_Switches_Status;
	}
	//if (ngxData.LTS_CircuitBreakerKnob != shareMemSDK->LTS_CircuitBreakerKnob) {
	//	ngxData.LTS_CircuitBreakerKnob = shareMemSDK->LTS_CircuitBreakerKnob;
	//	js["LTS_CircuitBreakerKnob"] = shareMemSDK->LTS_CircuitBreakerKnob;
	//}
	//if (ngxData.LTS_CircuitBreakerKnob != shareMemSDK->LTS_CircuitBreakerKnob) {
	//	ngxData.LTS_CircuitBreakerKnob = shareMemSDK->LTS_CircuitBreakerKnob;
	//	js["LTS_CircuitBreakerKnob"] = shareMemSDK->LTS_CircuitBreakerKnob;
	//}
	//if (ngxData.LTS_OvereadPanelKnob != shareMemSDK->LTS_OvereadPanelKnob) {
	//	ngxData.LTS_OvereadPanelKnob = shareMemSDK->LTS_OvereadPanelKnob;
	//	js["LTS_OvereadPanelKnob"] = shareMemSDK->LTS_OvereadPanelKnob;
	//}
	if (ngxData.Equipment_COOLING_SUPPLY_Switch_Status != shareMemSDK->Equipment_COOLING_SUPPLY_Switch_Status) {
		ngxData.Equipment_COOLING_SUPPLY_Switch_Status = shareMemSDK->Equipment_COOLING_SUPPLY_Switch_Status;
		js["AIR_EquipCoolingSupplyNORM"] = shareMemSDK->Equipment_COOLING_SUPPLY_Switch_Status != 0;
	}
	if (ngxData.Equipment_COOLING_EXHAUST_Switch_Status != shareMemSDK->Equipment_COOLING_EXHAUST_Switch_Status) {
		ngxData.Equipment_COOLING_EXHAUST_Switch_Status = shareMemSDK->Equipment_COOLING_EXHAUST_Switch_Status;
		js["AIR_EquipCoolingExhaustNORM"] = shareMemSDK->Equipment_COOLING_EXHAUST_Switch_Status != 0;
	}
//	if (ngxData.Emergency_Light_Switch_Status != shareMemSDK->Emergency_Light_Switch_Status) {
//		ngxData.Emergency_Light_Switch_Status = shareMemSDK->Emergency_Light_Switch_Status;
//		js["LTS_EmerExitSelector"] = shareMemSDK->Emergency_Light_Switch_Status;
//	}
	//if (ngxData.COMM_NoSmokingSelector != shareMemSDK->COMM_NoSmokingSelector) {
	//	ngxData.COMM_NoSmokingSelector = shareMemSDK->COMM_NoSmokingSelector;
	//	js["COMM_NoSmokingSelector"] = shareMemSDK->COMM_NoSmokingSelector != 0;
	//}
	if (ngxData.Fasten_Belts_Switches_Status != shareMemSDK->Fasten_Belts_Switches_Status) {
		ngxData.Fasten_Belts_Switches_Status = shareMemSDK->Fasten_Belts_Switches_Status;
		js["COMM_FastenBeltsSelector"] = shareMemSDK->Fasten_Belts_Switches_Status != 0;
	}
	if (ngxData.Wing_AntiIce_Switch_Status != shareMemSDK->Wing_AntiIce_Switch_Status) {
		ngxData.Wing_AntiIce_Switch_Status = shareMemSDK->Wing_AntiIce_Switch_Status;
		js["ICE_WingAntiIceSw"] = shareMemSDK->Wing_AntiIce_Switch_Status != 0;
	}
	//if (ngxData.AIR_TempSourceSelector != shareMemSDK->AIR_TempSourceSelector) {
	//	ngxData.AIR_TempSourceSelector = shareMemSDK->AIR_TempSourceSelector;
	//	js["AIR_TempSourceSelector"] = shareMemSDK->AIR_TempSourceSelector != 0;
	//}
	if (ngxData.TRIM_AIR_Switch_Status != shareMemSDK->TRIM_AIR_Switch_Status) {
		ngxData.TRIM_AIR_Switch_Status = shareMemSDK->TRIM_AIR_Switch_Status;
		js["AIR_TrimAirSwitch"] = shareMemSDK->TRIM_AIR_Switch_Status != 0;
	}
	if (ngxData.APU_Bleed_Air_Switches_Status != shareMemSDK->APU_Bleed_Air_Switches_Status) {
		ngxData.APU_Bleed_Air_Switches_Status = shareMemSDK->APU_Bleed_Air_Switches_Status;
		js["AIR_APUBleedAirSwitch"] = shareMemSDK->APU_Bleed_Air_Switches_Status != 0;
	}
	if (ngxData.Isolation_Valve_Switches_Status != shareMemSDK->Isolation_Valve_Switches_Status) {
		ngxData.Isolation_Valve_Switches_Status = shareMemSDK->Isolation_Valve_Switches_Status;
		js["AIR_IsolationValveSwitch"] = shareMemSDK->Isolation_Valve_Switches_Status;
	}
	//if (ngxData.AIR_OutflowValveSwitch != shareMemSDK->AIR_OutflowValveSwitch) {
	//	ngxData.AIR_OutflowValveSwitch = shareMemSDK->AIR_OutflowValveSwitch;
	//	js["AIR_OutflowValveSwitch"] = shareMemSDK->AIR_OutflowValveSwitch != 0;
	//}
	if (ngxData.Pressurization_Mode_Selector_Status != shareMemSDK->Pressurization_Mode_Selector_Status) {
		ngxData.Pressurization_Mode_Selector_Status = shareMemSDK->Pressurization_Mode_Selector_Status;
		js["AIR_PressurizationModeSelector"] = shareMemSDK->Pressurization_Mode_Selector_Status;
	}
	if (ngxData.Taxi_Light_Status != shareMemSDK->Taxi_Light_Status) {
		ngxData.Taxi_Light_Status = shareMemSDK->Taxi_Light_Status;
		js["LTS_TaxiSw"] = shareMemSDK->Taxi_Light_Status != 0;
	}
	if (ngxData.APU_Switches_Status != shareMemSDK->APU_Switches_Status) {
		ngxData.APU_Switches_Status = shareMemSDK->APU_Switches_Status;
		js["APU_Selector"] = shareMemSDK->APU_Switches_Status;
	}
	if (ngxData.Ignition_Select_Switches_Status != shareMemSDK->Ignition_Select_Switches_Status) {
		ngxData.Ignition_Select_Switches_Status = shareMemSDK->Ignition_Select_Switches_Status;
		js["ENG_IgnitionSelector"] = shareMemSDK->Ignition_Select_Switches_Status;
	}
	//if (ngxData.LTS_LogoSw != shareMemSDK->LTS_LogoSw) {
	//	ngxData.LTS_LogoSw = shareMemSDK->LTS_LogoSw;
	//	js["LTS_LogoSw"] = shareMemSDK->LTS_LogoSw != 0;
	//}
	if (ngxData.Position_Light_Status != shareMemSDK->Position_Light_Status) {
		ngxData.Position_Light_Status = shareMemSDK->Position_Light_Status;
		js["LTS_PositionSw"] = shareMemSDK->Position_Light_Status;
	}
	if (ngxData.Anti_Collision_Light_Status != shareMemSDK->Anti_Collision_Light_Status) {
		ngxData.Anti_Collision_Light_Status = shareMemSDK->Anti_Collision_Light_Status;
		js["LTS_AntiCollisionSw"] = shareMemSDK->Anti_Collision_Light_Status;
	}
	if (ngxData.Wing_Light_Status != shareMemSDK->Wing_Light_Status) {
		ngxData.Wing_Light_Status = shareMemSDK->Wing_Light_Status;
		js["LTS_WingSw"] = shareMemSDK->Wing_Light_Status != 0;
	}
	if (ngxData.WHEEL_WELL_Light_Status != shareMemSDK->WHEEL_WELL_Light_Status) {
		ngxData.WHEEL_WELL_Light_Status = shareMemSDK->WHEEL_WELL_Light_Status;
		js["LTS_WheelWellSw"] = shareMemSDK->WHEEL_WELL_Light_Status != 0;
	}
//	if (ngxData.MAIN_NoseWheelSteeringSwNORM != shareMemSDK->MAIN_NoseWheelSteeringSwNORM) {
//		ngxData.MAIN_NoseWheelSteeringSwNORM = shareMemSDK->MAIN_NoseWheelSteeringSwNORM;
//		js["MAIN_NoseWheelSteeringSwNORM"] = shareMemSDK->MAIN_NoseWheelSteeringSwNORM != 0;
//	}
	//if (ngxData.MAIN_LightsSelector != shareMemSDK->MAIN_LightsSelector) {
	//	ngxData.MAIN_LightsSelector = shareMemSDK->MAIN_LightsSelector;
	//	js["MAIN_LightsSelector"] = shareMemSDK->MAIN_LightsSelector != 0;
	//}
	//if (ngxData.MAIN_RMISelector1_VOR != shareMemSDK->MAIN_RMISelector1_VOR) {
	//	ngxData.MAIN_RMISelector1_VOR = shareMemSDK->MAIN_RMISelector1_VOR;
	//	js["MAIN_RMISelector1_VOR"] = shareMemSDK->MAIN_RMISelector1_VOR != 0;
	//}
	//if (ngxData.MAIN_RMISelector2_VOR != shareMemSDK->MAIN_RMISelector2_VOR) {
	//	ngxData.MAIN_RMISelector2_VOR = shareMemSDK->MAIN_RMISelector2_VOR;
	//	js["MAIN_RMISelector2_VOR"] = shareMemSDK->MAIN_RMISelector2_VOR != 0;
	//}
	if (ngxData.N1_Set_Status != shareMemSDK->N1_Set_Status) {
		ngxData.N1_Set_Status = shareMemSDK->N1_Set_Status;
		js["MAIN_N1SetSelector"] = shareMemSDK->N1_Set_Status;
	}
	if (ngxData.N1_Set_Status != shareMemSDK->N1_Set_Status) {
		ngxData.N1_Set_Status = shareMemSDK->N1_Set_Status;
		js["MAIN_SpdRefSelector"] = shareMemSDK->N1_Set_Status;
	}
	if (ngxData.Fuel_Flow_Status != shareMemSDK->Fuel_Flow_Status) {
		ngxData.Fuel_Flow_Status = shareMemSDK->Fuel_Flow_Status;
		js["MAIN_FuelFlowSelector"] = shareMemSDK->Fuel_Flow_Status;
	}
	if (ngxData.Autobrake_Switches_Status != shareMemSDK->Autobrake_Switches_Status) {
		ngxData.Autobrake_Switches_Status = shareMemSDK->Autobrake_Switches_Status;
		js["MAIN_AutobrakeSelector"] = shareMemSDK->Autobrake_Switches_Status;
	}
	if (ngxData.Gear_Lever_Status != shareMemSDK->Gear_Lever_Status) {
		ngxData.Gear_Lever_Status = shareMemSDK->Gear_Lever_Status;
		js["MAIN_GearLever"] = shareMemSDK->Gear_Lever_Status;
	}
	if (ngxData.Flap_Inhibit_Switches_Status != shareMemSDK->Flap_Inhibit_Switches_Status) {
		ngxData.Flap_Inhibit_Switches_Status = shareMemSDK->Flap_Inhibit_Switches_Status;
		js["GPWS_FlapInhibitSw_NORM"] = shareMemSDK->Flap_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Gear_Inhibit_Switches_Status != shareMemSDK->Gear_Inhibit_Switches_Status) {
		ngxData.Gear_Inhibit_Switches_Status = shareMemSDK->Gear_Inhibit_Switches_Status;
		js["GPWS_GearInhibitSw_NORM"] = shareMemSDK->Gear_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Terr_Inhibit_Switches_Status != shareMemSDK->Terr_Inhibit_Switches_Status) {
		ngxData.Terr_Inhibit_Switches_Status = shareMemSDK->Terr_Inhibit_Switches_Status;
		js["GPWS_TerrInhibitSw_NORM"] = shareMemSDK->Terr_Inhibit_Switches_Status != 0;
	}
	if (ngxData.Stab_Trim_Main_Elect_Switch_Status != shareMemSDK->Stab_Trim_Main_Elect_Switch_Status) {
		ngxData.Stab_Trim_Main_Elect_Switch_Status = shareMemSDK->Stab_Trim_Main_Elect_Switch_Status;
		js["TRIM_StabTrimMainElecSw_NORMAL"] = shareMemSDK->Stab_Trim_Main_Elect_Switch_Status != 0;
	}
	if (ngxData.Stab_Trim_Auto_Pilot_Switch_Status != shareMemSDK->Stab_Trim_Auto_Pilot_Switch_Status) {
		ngxData.Stab_Trim_Auto_Pilot_Switch_Status = shareMemSDK->Stab_Trim_Auto_Pilot_Switch_Status;
		js["TRIM_StabTrimAutoPilotSw_NORMAL"] = shareMemSDK->Stab_Trim_Auto_Pilot_Switch_Status != 0;
	}
	//if (ngxData.FIRE_DetTestSw != shareMemSDK->FIRE_DetTestSw) {
	//	ngxData.FIRE_DetTestSw = shareMemSDK->FIRE_DetTestSw;
	//	js["FIRE_DetTestSw"] = shareMemSDK->FIRE_DetTestSw != 0;
	//}
	//if (ngxData.FIRE_ExtinguisherTestSw != shareMemSDK->FIRE_ExtinguisherTestSw) {
	//	ngxData.FIRE_ExtinguisherTestSw = shareMemSDK->FIRE_ExtinguisherTestSw;
	//	js["FIRE_ExtinguisherTestSw"] = shareMemSDK->FIRE_ExtinguisherTestSw != 0;
	//}
	//if (ngxData.XPDR_XpndrSelector_2 != shareMemSDK->XPDR_XpndrSelector_2) {
	//	ngxData.XPDR_XpndrSelector_2 = shareMemSDK->XPDR_XpndrSelector_2;
	//	js["XPDR_XpndrSelector_2"] = shareMemSDK->XPDR_XpndrSelector_2 != 0;
	//}
	//if (ngxData.XPDR_AltSourceSel_2 != shareMemSDK->XPDR_AltSourceSel_2) {
	//	ngxData.XPDR_AltSourceSel_2 = shareMemSDK->XPDR_AltSourceSel_2;
	//	js["XPDR_AltSourceSel_2"] = shareMemSDK->XPDR_AltSourceSel_2 != 0;
	//}
	//if (ngxData.XPDR_ModeSel != shareMemSDK->XPDR_ModeSel) {
	//	ngxData.XPDR_ModeSel = shareMemSDK->XPDR_ModeSel;
	//	js["XPDR_ModeSel"] = shareMemSDK->XPDR_ModeSel != 0;
	//}
	if (ngxData.Stabilizer_Trim_Switch_Status != shareMemSDK->Stabilizer_Trim_Switch_Status) {
		ngxData.Stabilizer_Trim_Switch_Status = shareMemSDK->Stabilizer_Trim_Switch_Status;
		js["TRIM_StabTrimSw_NORMAL"] = shareMemSDK->Stabilizer_Trim_Switch_Status != 0;
	}
	//if (ngxData.PED_FltDkDoorSel != shareMemSDK->PED_FltDkDoorSel) {
	//	ngxData.PED_FltDkDoorSel = shareMemSDK->PED_FltDkDoorSel;
	//	js["PED_FltDkDoorSel"] = shareMemSDK->PED_FltDkDoorSel != 0;
	//}
	if (ngxData.IRU_1_STATUE != shareMemSDK->IRU_1_STATUE || ngxData.IRU_2_STATUE != shareMemSDK->IRU_2_STATUE) {
		ngxData.IRU_1_STATUE = shareMemSDK->IRU_1_STATUE;
		ngxData.IRU_2_STATUE = shareMemSDK->IRU_2_STATUE;
		js["IRS_aligned"] = shareMemSDK->IRU_1_STATUE == 2 && shareMemSDK->IRU_2_STATUE == 2;
	}
	//if (ngxData.AircraftModel != shareMemSDK->AircraftModel) {
	//	ngxData.AircraftModel = shareMemSDK->AircraftModel;
	//	js["AircraftModel"] = shareMemSDK->AircraftModel != 0;
	//}
	//if (ngxData.WeightInKg != shareMemSDK->WeightInKg) {
	//	ngxData.WeightInKg = shareMemSDK->WeightInKg;
	//	js["WeightInKg"] = shareMemSDK->WeightInKg != 0;
	//}
	//if (ngxData.GPWS_V1CallEnabled != shareMemSDK->GPWS_V1CallEnabled) {
	//	ngxData.GPWS_V1CallEnabled = shareMemSDK->GPWS_V1CallEnabled;
	//	js["GPWS_V1CallEnabled"] = shareMemSDK->GPWS_V1CallEnabled != 0;
	//}
	//if (ngxData.GroundConnAvailable != shareMemSDK->GroundConnAvailable) {
	//	ngxData.GroundConnAvailable = shareMemSDK->GroundConnAvailable;
	//	js["GroundConnAvailable"] = shareMemSDK->GroundConnAvailable != 0;
	//}
	//if (ngxData.FMC_TakeoffFlaps != shareMemSDK->FMC_TakeoffFlaps) {
	//	ngxData.FMC_TakeoffFlaps = shareMemSDK->FMC_TakeoffFlaps;
	//	js["FMC_TakeoffFlaps"] = shareMemSDK->FMC_TakeoffFlaps != 0;
	//}
	//if (ngxData.FMC_V1 != shareMemSDK->FMC_V1) {
	//	ngxData.FMC_V1 = shareMemSDK->FMC_V1;
	//	js["FMC_V1"] = shareMemSDK->FMC_V1 != 0;
	//}
	//if (ngxData.FMC_VR != shareMemSDK->FMC_VR) {
	//	ngxData.FMC_VR = shareMemSDK->FMC_VR;
	//	js["FMC_VR"] = shareMemSDK->FMC_VR != 0;
	//}
	//if (ngxData.FMC_V2 != shareMemSDK->FMC_V2) {
	//	ngxData.FMC_V2 = shareMemSDK->FMC_V2;
	//	js["FMC_V2"] = shareMemSDK->FMC_V2 != 0;
	//}
	//if (ngxData.FMC_LandingFlaps != shareMemSDK->FMC_LandingFlaps) {
	//	ngxData.FMC_LandingFlaps = shareMemSDK->FMC_LandingFlaps;
	//	js["FMC_LandingFlaps"] = shareMemSDK->FMC_LandingFlaps != 0;
	//}
	//if (ngxData.FMC_LandingVREF != shareMemSDK->FMC_LandingVREF) {
	//	ngxData.FMC_LandingVREF = shareMemSDK->FMC_LandingVREF;
	//	js["FMC_LandingVREF"] = shareMemSDK->FMC_LandingVREF != 0;
	//}
	//if (ngxData.FMC_CruiseAlt != shareMemSDK->FMC_CruiseAlt) {
	//	ngxData.FMC_CruiseAlt = shareMemSDK->FMC_CruiseAlt;
	//	js["FMC_CruiseAlt"] = shareMemSDK->FMC_CruiseAlt != 0;
	//}
	//if (ngxData.FMC_LandingAltitude != shareMemSDK->FMC_LandingAltitude) {
	//	ngxData.FMC_LandingAltitude = shareMemSDK->FMC_LandingAltitude;
	//	js["FMC_LandingAltitude"] = shareMemSDK->FMC_LandingAltitude != 0;
	//}
	//if (ngxData.FMC_TransitionAlt != shareMemSDK->FMC_TransitionAlt) {
	//	ngxData.FMC_TransitionAlt = shareMemSDK->FMC_TransitionAlt;
	//	js["FMC_TransitionAlt"] = shareMemSDK->FMC_TransitionAlt != 0;
	//}
	//if (ngxData.FMC_TransitionLevel != shareMemSDK->FMC_TransitionLevel) {
	//	ngxData.FMC_TransitionLevel = shareMemSDK->FMC_TransitionLevel;
	//	js["FMC_TransitionLevel"] = shareMemSDK->FMC_TransitionLevel != 0;
	//}
	//if (ngxData.FMC_PerfInputComplete != shareMemSDK->FMC_PerfInputComplete) {
	//	ngxData.FMC_PerfInputComplete = shareMemSDK->FMC_PerfInputComplete;
	//	js["FMC_PerfInputComplete"] = shareMemSDK->FMC_PerfInputComplete != 0;
	//}
	//if (ngxData.FMC_DistanceToTOD != shareMemSDK->FMC_DistanceToTOD) {
	//	ngxData.FMC_DistanceToTOD = shareMemSDK->FMC_DistanceToTOD;
	//	js["FMC_DistanceToTOD"] = shareMemSDK->FMC_DistanceToTOD != 0;
	//}
	//if (ngxData.FMC_DistanceToDest != shareMemSDK->FMC_DistanceToDest) {
	//	ngxData.FMC_DistanceToDest = shareMemSDK->FMC_DistanceToDest;
	//	js["FMC_DistanceToDest"] = shareMemSDK->FMC_DistanceToDest != 0;
	//}
	//if (ngxData.ICE_WindowHeatTestSw != shareMemSDK->ICE_WindowHeatTestSw) {
	//	ngxData.ICE_WindowHeatTestSw = shareMemSDK->ICE_WindowHeatTestSw;
	//	js["ICE_WindowHeatTestSw"] = shareMemSDK->ICE_WindowHeatTestSw != 0;
	//}


	if (ngxData.IRS_1_Switches_Status != shareMemSDK->IRS_1_Switches_Status) {
		ngxData.IRS_1_Switches_Status = shareMemSDK->IRS_1_Switches_Status;
		js["IRS_ModeSelector_1"] = shareMemSDK->IRS_1_Switches_Status;
	}
	if (ngxData.IRS_2_Switches_Status != shareMemSDK->IRS_2_Switches_Status) {
		ngxData.IRS_2_Switches_Status = shareMemSDK->IRS_2_Switches_Status;
		js["IRS_ModeSelector_2"] = shareMemSDK->IRS_2_Switches_Status;
	}
	if (ngxData.EEC_1_Switch_Status != shareMemSDK->EEC_1_Switch_Status) {
		ngxData.EEC_1_Switch_Status = shareMemSDK->EEC_1_Switch_Status;
		js["ENG_EECSwitch_1"] = shareMemSDK->EEC_1_Switch_Status == 2;
	}
	if (ngxData.EEC_2_Switch_Status != shareMemSDK->EEC_2_Switch_Status) {
		ngxData.EEC_2_Switch_Status = shareMemSDK->EEC_2_Switch_Status;
		js["ENG_EECSwitch_2"] = shareMemSDK->EEC_2_Switch_Status == 2;
	}
	if (ngxData.Flight_Control_A_Switches_Status != shareMemSDK->Flight_Control_A_Switches_Status) {
		ngxData.Flight_Control_A_Switches_Status = shareMemSDK->Flight_Control_A_Switches_Status;
		js["FCTL_FltControl_Sw_1"] = shareMemSDK->Flight_Control_A_Switches_Status;
	}
	if (ngxData.Flight_Control_B_Switches_Status != shareMemSDK->Flight_Control_B_Switches_Status) {
		ngxData.Flight_Control_B_Switches_Status = shareMemSDK->Flight_Control_B_Switches_Status;
		js["FCTL_FltControl_Sw_2"] = shareMemSDK->Flight_Control_B_Switches_Status;
	}
	if (ngxData.Spoiler_A_Switches_Status != shareMemSDK->Spoiler_A_Switches_Status) {
		ngxData.Spoiler_A_Switches_Status = shareMemSDK->Spoiler_A_Switches_Status;
		js["FCTL_Spoiler_Sw_1"] = shareMemSDK->Spoiler_A_Switches_Status != 0;
	}
	if (ngxData.Spoiler_B_Switches_Status != shareMemSDK->Spoiler_B_Switches_Status) {
		ngxData.Spoiler_B_Switches_Status = shareMemSDK->Spoiler_B_Switches_Status;
		js["FCTL_Spoiler_Sw_2"] = shareMemSDK->Spoiler_B_Switches_Status != 0;
	}
	if (ngxData.Fuel_L_FWD_Switch_Status != shareMemSDK->Fuel_L_FWD_Switch_Status) {
		ngxData.Fuel_L_FWD_Switch_Status = shareMemSDK->Fuel_L_FWD_Switch_Status;
		js["FUEL_PumpFwdSw_1"] = shareMemSDK->Fuel_L_FWD_Switch_Status != 0;
	}
	if (ngxData.Fuel_R_FWD_Switch_Status != shareMemSDK->Fuel_R_FWD_Switch_Status) {
		ngxData.Fuel_R_FWD_Switch_Status = shareMemSDK->Fuel_R_FWD_Switch_Status;
		js["FUEL_PumpFwdSw_2"] = shareMemSDK->Fuel_R_FWD_Switch_Status != 0;
	}
	if (ngxData.Fuel_L_AFT_Switch_Status != shareMemSDK->Fuel_L_AFT_Switch_Status) {
		ngxData.Fuel_L_AFT_Switch_Status = shareMemSDK->Fuel_L_AFT_Switch_Status;
		js["FUEL_PumpAftSw_1"] = shareMemSDK->Fuel_L_AFT_Switch_Status != 0;
	}
	if (ngxData.Fuel_R_AFT_Switch_Status != shareMemSDK->Fuel_R_AFT_Switch_Status) {
		ngxData.Fuel_R_AFT_Switch_Status = shareMemSDK->Fuel_R_AFT_Switch_Status;
		js["FUEL_PumpAftSw_2"] = shareMemSDK->Fuel_R_AFT_Switch_Status != 0;
	}
	if (ngxData.Fuel_CENTER_L_Switch_Status != shareMemSDK->Fuel_CENTER_L_Switch_Status) {
		ngxData.Fuel_CENTER_L_Switch_Status = shareMemSDK->Fuel_CENTER_L_Switch_Status;
		js["FUEL_PumpCtrSw_1"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	}
	if (ngxData.Fuel_CENTER_L_Switch_Status != shareMemSDK->Fuel_CENTER_L_Switch_Status) {
		ngxData.Fuel_CENTER_L_Switch_Status = shareMemSDK->Fuel_CENTER_L_Switch_Status;
		js["FUEL_PumpCtrSw_2"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	}
	if (ngxData.Generator_1_Drive_Disconnect_Switch_Status != shareMemSDK->Generator_1_Drive_Disconnect_Switch_Status) {
		ngxData.Generator_1_Drive_Disconnect_Switch_Status = shareMemSDK->Generator_1_Drive_Disconnect_Switch_Status;
		js["ELEC_IDGDisconnectSw_1"] = shareMemSDK->Generator_1_Drive_Disconnect_Switch_Status != 0;
	}
	if (ngxData.Generator_2_Drive_Disconnect_Switch_Status != shareMemSDK->Generator_2_Drive_Disconnect_Switch_Status) {
		ngxData.Generator_2_Drive_Disconnect_Switch_Status = shareMemSDK->Generator_2_Drive_Disconnect_Switch_Status;
		js["ELEC_IDGDisconnectSw_2"] = shareMemSDK->Generator_2_Drive_Disconnect_Switch_Status != 0;
	}
	//if (ngxData.ELEC_GenSw[0] != shareMemSDK->ELEC_GenSw[0]) {
	//	ngxData.ELEC_GenSw[0] = shareMemSDK->ELEC_GenSw[0];
	//	js["ELEC_GenSw_1"] = shareMemSDK->ELEC_GenSw[0] != 0;
	//}
	//if (ngxData.ELEC_GenSw[1] != shareMemSDK->ELEC_GenSw[1]) {
	//	ngxData.ELEC_GenSw[1] = shareMemSDK->ELEC_GenSw[1];
	//	js["ELEC_GenSw_2"] = shareMemSDK->ELEC_GenSw[1] != 0;
	//}
	if (ngxData.APU_Generator_1_Switches_Status != shareMemSDK->APU_Generator_1_Switches_Status) {
		ngxData.APU_Generator_1_Switches_Status = shareMemSDK->APU_Generator_1_Switches_Status;
		js["ELEC_APUGenSw_1"] = shareMemSDK->APU_Generator_1_Switches_Status != 0;
	}
	if (ngxData.APU_Generator_2_Switches_Status != shareMemSDK->APU_Generator_2_Switches_Status) {
		ngxData.APU_Generator_2_Switches_Status = shareMemSDK->APU_Generator_2_Switches_Status;
		js["ELEC_APUGenSw_2"] = shareMemSDK->APU_Generator_2_Switches_Status != 0;
	}
	if (ngxData.Window_Heat_Switch_1_Status != shareMemSDK->Window_Heat_Switch_1_Status) {
		ngxData.Window_Heat_Switch_1_Status = shareMemSDK->Window_Heat_Switch_1_Status;
		js["ICE_WindowHeatSw_1"] = shareMemSDK->Window_Heat_Switch_1_Status != 0;
	}
	if (ngxData.Window_Heat_Switch_2_Status != shareMemSDK->Window_Heat_Switch_2_Status) {
		ngxData.Window_Heat_Switch_2_Status = shareMemSDK->Window_Heat_Switch_2_Status;
		js["ICE_WindowHeatSw_2"] = shareMemSDK->Window_Heat_Switch_2_Status != 0;
	}
	if (ngxData.Window_Heat_Switch_3_Status != shareMemSDK->Window_Heat_Switch_3_Status) {
		ngxData.Window_Heat_Switch_3_Status = shareMemSDK->Window_Heat_Switch_3_Status;
		js["ICE_WindowHeatSw_3"] = shareMemSDK->Window_Heat_Switch_3_Status != 0;
	}
	if (ngxData.Window_Heat_Switch_4_Status != shareMemSDK->Window_Heat_Switch_4_Status) {
		ngxData.Window_Heat_Switch_4_Status = shareMemSDK->Window_Heat_Switch_4_Status;
		js["ICE_WindowHeatSw_4"] = shareMemSDK->Window_Heat_Switch_4_Status != 0;
	}

	if (ngxData.Probe_Heat_Switch_1_Status != shareMemSDK->Probe_Heat_Switch_1_Status) {
		ngxData.Probe_Heat_Switch_1_Status = shareMemSDK->Probe_Heat_Switch_1_Status;
		js["ICE_ProbeHeatSw_1"] = shareMemSDK->Probe_Heat_Switch_1_Status != 0;
	}
	if (ngxData.Probe_Heat_Switch_2_Status != shareMemSDK->Probe_Heat_Switch_2_Status) {
		ngxData.Probe_Heat_Switch_2_Status = shareMemSDK->Probe_Heat_Switch_2_Status;
		js["ICE_ProbeHeatSw_2"] = shareMemSDK->Probe_Heat_Switch_2_Status != 0;
	}
	if (ngxData.Eng_1_AntiIce_Switch_Status != shareMemSDK->Eng_1_AntiIce_Switch_Status) {
		ngxData.Eng_1_AntiIce_Switch_Status = shareMemSDK->Eng_1_AntiIce_Switch_Status;
		js["ICE_EngAntiIceSw_1"] = shareMemSDK->Eng_1_AntiIce_Switch_Status != 0;
	}
	if (ngxData.Eng_2_AntiIce_Switch_Status != shareMemSDK->Eng_2_AntiIce_Switch_Status) {
		ngxData.Eng_2_AntiIce_Switch_Status = shareMemSDK->Eng_2_AntiIce_Switch_Status;
		js["ICE_EngAntiIceSw_2"] = shareMemSDK->Eng_2_AntiIce_Switch_Status != 0;
	}
	if (ngxData.ENG_1_HYD_Switches_Status != shareMemSDK->ENG_1_HYD_Switches_Status) {
		ngxData.ENG_1_HYD_Switches_Status = shareMemSDK->ENG_1_HYD_Switches_Status;
		js["HYD_PumpSw_eng_1"] = shareMemSDK->ENG_1_HYD_Switches_Status != 0;
	}
	if (ngxData.ENG_2_HYD_Switches_Status != shareMemSDK->ENG_2_HYD_Switches_Status) {
		ngxData.ENG_2_HYD_Switches_Status = shareMemSDK->ENG_2_HYD_Switches_Status;
		js["HYD_PumpSw_eng_2"] = shareMemSDK->ENG_2_HYD_Switches_Status != 0;
	}
	if (ngxData.ELEC_1_HYD_Switches_Status != shareMemSDK->ELEC_1_HYD_Switches_Status) {
		ngxData.ELEC_1_HYD_Switches_Status = shareMemSDK->ELEC_1_HYD_Switches_Status;
		js["HYD_PumpSw_elec_1"] = shareMemSDK->ELEC_1_HYD_Switches_Status != 0;
	}
	if (ngxData.ELEC_2_HYD_Switches_Status != shareMemSDK->ELEC_2_HYD_Switches_Status) {
		ngxData.ELEC_2_HYD_Switches_Status = shareMemSDK->ELEC_2_HYD_Switches_Status;
		js["HYD_PumpSw_elec_2"] = shareMemSDK->ELEC_2_HYD_Switches_Status != 0;
	}
	if (ngxData.RecircFan_L_Switch_Status != shareMemSDK->RecircFan_L_Switch_Status) {
		ngxData.RecircFan_L_Switch_Status = shareMemSDK->RecircFan_L_Switch_Status;
		js["AIR_RecircFanSwitch_1"] = shareMemSDK->RecircFan_L_Switch_Status != 0;
	}
	if (ngxData.RecircFan_R_Switch_Status != shareMemSDK->RecircFan_R_Switch_Status) {
		ngxData.RecircFan_R_Switch_Status = shareMemSDK->RecircFan_R_Switch_Status;
		js["AIR_RecircFanSwitch_2"] = shareMemSDK->RecircFan_R_Switch_Status != 0;
	}
	if (ngxData.Pack_1_Switches_Status != shareMemSDK->Pack_1_Switches_Status) {
		ngxData.Pack_1_Switches_Status = shareMemSDK->Pack_1_Switches_Status;
		js["AIR_PackSwitch_1"] = shareMemSDK->Pack_1_Switches_Status;
	}
	if (ngxData.Pack_2_Switches_Status != shareMemSDK->Pack_2_Switches_Status) {
		ngxData.Pack_2_Switches_Status = shareMemSDK->Pack_2_Switches_Status;
		js["AIR_PackSwitch_2"] = shareMemSDK->Pack_2_Switches_Status;
	}
	if (ngxData.Engine_1_Bleed_Air_Switches_Status != shareMemSDK->Engine_1_Bleed_Air_Switches_Status) {
		ngxData.Engine_1_Bleed_Air_Switches_Status = shareMemSDK->Engine_1_Bleed_Air_Switches_Status;
		js["AIR_BleedAirSwitch_1"] = shareMemSDK->Engine_1_Bleed_Air_Switches_Status != 0;
	}
	if (ngxData.Engine_2_Bleed_Air_Switches_Status != shareMemSDK->Engine_2_Bleed_Air_Switches_Status) {
		ngxData.Engine_2_Bleed_Air_Switches_Status = shareMemSDK->Engine_2_Bleed_Air_Switches_Status;
		js["AIR_BleedAirSwitch_2"] = shareMemSDK->Engine_2_Bleed_Air_Switches_Status != 0;
	}
	if (ngxData.Landing_Light_1_Status != shareMemSDK->Landing_Light_1_Status) {
		ngxData.Landing_Light_1_Status = shareMemSDK->Landing_Light_1_Status;
		js["LTS_LandingLtRetractableSw_1"] = shareMemSDK->Landing_Light_1_Status;
	}
	if (ngxData.Landing_Light_2_Status != shareMemSDK->Landing_Light_2_Status) {
		ngxData.Landing_Light_2_Status = shareMemSDK->Landing_Light_2_Status;
		js["LTS_LandingLtRetractableSw_2"] = shareMemSDK->Landing_Light_2_Status;
	}
	if (ngxData.Landing_Light_3_Status != shareMemSDK->Landing_Light_3_Status) {
		ngxData.Landing_Light_3_Status = shareMemSDK->Landing_Light_3_Status;
		js["LTS_LandingLtFixedSw_1"] = shareMemSDK->Landing_Light_3_Status != 0;
	}
	if (ngxData.Landing_Light_4_Status != shareMemSDK->Landing_Light_4_Status) {
		ngxData.Landing_Light_4_Status = shareMemSDK->Landing_Light_4_Status;
		js["LTS_LandingLtFixedSw_2"] = shareMemSDK->Landing_Light_4_Status != 0;
	}
	if (ngxData.Runway_Turnoff_Light_Status != shareMemSDK->Runway_Turnoff_Light_Status) {
		ngxData.Runway_Turnoff_Light_Status = shareMemSDK->Runway_Turnoff_Light_Status;
		js["LTS_RunwayTurnoffSw_1"] = shareMemSDK->Runway_Turnoff_Light_Status != 0;
	}
	if (ngxData.Runway_Turnoff_Light_Status != shareMemSDK->Runway_Turnoff_Light_Status) {
		ngxData.Runway_Turnoff_Light_Status = shareMemSDK->Runway_Turnoff_Light_Status;
		js["LTS_RunwayTurnoffSw_2"] = shareMemSDK->Runway_Turnoff_Light_Status != 0;
	}
	if (ngxData.Engine_1_Start_Switch_Status != shareMemSDK->Engine_1_Start_Switch_Status) {
		ngxData.Engine_1_Start_Switch_Status = shareMemSDK->Engine_1_Start_Switch_Status;
		js["ENG_StartSelector_1"] = shareMemSDK->Engine_1_Start_Switch_Status;
	}
	if (ngxData.Engine_2_Start_Switch_Status != shareMemSDK->Engine_2_Start_Switch_Status) {
		ngxData.Engine_2_Start_Switch_Status = shareMemSDK->Engine_2_Start_Switch_Status;
		js["ENG_StartSelector_2"] = shareMemSDK->Engine_2_Start_Switch_Status;
	}
	//if (ngxData.EFIS_MinsSelBARO[0] != shareMemSDK->EFIS_MinsSelBARO[0]) {
	//	ngxData.EFIS_MinsSelBARO[0] = shareMemSDK->EFIS_MinsSelBARO[0];
	//	js["EFIS_MinsSelBARO_1"] = shareMemSDK->EFIS_MinsSelBARO[0] != 0;
	//}
	//if (ngxData.EFIS_MinsSelBARO[1] != shareMemSDK->EFIS_MinsSelBARO[1]) {
	//	ngxData.EFIS_MinsSelBARO[1] = shareMemSDK->EFIS_MinsSelBARO[1];
	//	js["EFIS_MinsSelBARO_2"] = shareMemSDK->EFIS_MinsSelBARO[1] != 0;
	//}
	//if (ngxData.EFIS_BaroSelHPA[0] != shareMemSDK->EFIS_BaroSelHPA[0]) {
	//	ngxData.EFIS_BaroSelHPA[0] = shareMemSDK->EFIS_BaroSelHPA[0];
	//	js["EFIS_BaroSelHPA_1"] = shareMemSDK->EFIS_BaroSelHPA[0] != 0;
	//}
	//if (ngxData.EFIS_BaroSelHPA[1] != shareMemSDK->EFIS_BaroSelHPA[1]) {
	//	ngxData.EFIS_BaroSelHPA[1] = shareMemSDK->EFIS_BaroSelHPA[1];
	//	js["EFIS_BaroSelHPA_2"] = shareMemSDK->EFIS_BaroSelHPA[1] != 0;
	//}
	if (ngxData.Main_Panel_DU_CAPT_Switches_Status != shareMemSDK->Main_Panel_DU_CAPT_Switches_Status) {
		ngxData.Main_Panel_DU_CAPT_Switches_Status = shareMemSDK->Main_Panel_DU_CAPT_Switches_Status;
		js["MAIN_MainPanelDUSel_1"] = shareMemSDK->Main_Panel_DU_CAPT_Switches_Status;
	}
	if (ngxData.Main_Panel_DU_FO_Switches_Status != shareMemSDK->Main_Panel_DU_FO_Switches_Status) {
		ngxData.Main_Panel_DU_FO_Switches_Status = shareMemSDK->Main_Panel_DU_FO_Switches_Status;
		js["MAIN_MainPanelDUSel_2"] = shareMemSDK->Main_Panel_DU_FO_Switches_Status;
	}
	if (ngxData.Lower_DU_Switches_Status != shareMemSDK->Lower_DU_Switches_Status) {
		ngxData.Lower_DU_Switches_Status = shareMemSDK->Lower_DU_Switches_Status;
		js["MAIN_LowerDUSel_1"] = shareMemSDK->Lower_DU_Switches_Status;
	}
	if (ngxData.Lower_DU_Switches_Status != shareMemSDK->Lower_DU_Switches_Status) {
		ngxData.Lower_DU_Switches_Status = shareMemSDK->Lower_DU_Switches_Status;
		js["MAIN_LowerDUSel_2"] = shareMemSDK->Lower_DU_Switches_Status;
	}
	//if (ngxData.MAIN_DisengageTestSelector[0] != shareMemSDK->MAIN_DisengageTestSelector[0]) {
	//	ngxData.MAIN_DisengageTestSelector[0] = shareMemSDK->MAIN_DisengageTestSelector[0];
	//	js["MAIN_DisengageTestSelector_1"] = shareMemSDK->MAIN_DisengageTestSelector[0] != 0;
	//}
	//if (ngxData.MAIN_DisengageTestSelector[1] != shareMemSDK->MAIN_DisengageTestSelector[1]) {
	//	ngxData.MAIN_DisengageTestSelector[1] = shareMemSDK->MAIN_DisengageTestSelector[1];
	//	js["MAIN_DisengageTestSelector_2"] = shareMemSDK->MAIN_DisengageTestSelector[1] != 0;
	//}
	if (ngxData.OVHT_DET_1_Switches_Status != shareMemSDK->OVHT_DET_1_Switches_Status) {
		ngxData.OVHT_DET_1_Switches_Status = shareMemSDK->OVHT_DET_1_Switches_Status;
		js["FIRE_OvhtDetSw_1"] = shareMemSDK->OVHT_DET_1_Switches_Status;
	}
	if (ngxData.OVHT_DET_2_Switches_Status != shareMemSDK->OVHT_DET_2_Switches_Status) {
		ngxData.OVHT_DET_2_Switches_Status = shareMemSDK->OVHT_DET_2_Switches_Status;
		js["FIRE_OvhtDetSw_2"] = shareMemSDK->OVHT_DET_2_Switches_Status;
	}
	//if (ngxData.FIRE_HandlePos[0] != shareMemSDK->FIRE_HandlePos[0]) {
	//	ngxData.FIRE_HandlePos[0] = shareMemSDK->FIRE_HandlePos[0];
	//	js["FIRE_HandlePos_1"] = shareMemSDK->FIRE_HandlePos[0] != 0;
	//}
	//if (ngxData.FIRE_HandlePos[1] != shareMemSDK->FIRE_HandlePos[1]) {
	//	ngxData.FIRE_HandlePos[1] = shareMemSDK->FIRE_HandlePos[1];
	//	js["FIRE_HandlePos_2"] = shareMemSDK->FIRE_HandlePos[1] != 0;
	//}
	//if (ngxData.FIRE_HandlePos[2] != shareMemSDK->FIRE_HandlePos[2]) {
	//	ngxData.FIRE_HandlePos[2] = shareMemSDK->FIRE_HandlePos[2];
	//	js["FIRE_HandlePos_3"] = shareMemSDK->FIRE_HandlePos[2] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[0] != shareMemSDK->FIRE_HandleIlluminated[0]) {
	//	ngxData.FIRE_HandleIlluminated[0] = shareMemSDK->FIRE_HandleIlluminated[0];
	//	js["FIRE_HandleIlluminated_1"] = shareMemSDK->FIRE_HandleIlluminated[0] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[1] != shareMemSDK->FIRE_HandleIlluminated[1]) {
	//	ngxData.FIRE_HandleIlluminated[1] = shareMemSDK->FIRE_HandleIlluminated[1];
	//	js["FIRE_HandleIlluminated_2"] = shareMemSDK->FIRE_HandleIlluminated[1] != 0;
	//}
	//if (ngxData.FIRE_HandleIlluminated[2] != shareMemSDK->FIRE_HandleIlluminated[2]) {
	//	ngxData.FIRE_HandleIlluminated[2] = shareMemSDK->FIRE_HandleIlluminated[2];
	//	js["FIRE_HandleIlluminated_3"] = shareMemSDK->FIRE_HandleIlluminated[2] != 0;
	//}
	if (ngxData.DET_Select_1_Switch_Status != shareMemSDK->DET_Select_1_Switch_Status) {
		ngxData.DET_Select_1_Switch_Status = shareMemSDK->DET_Select_1_Switch_Status;
		js["CARGO_DetSelect_1"] = shareMemSDK->DET_Select_1_Switch_Status;
	}
	if (ngxData.DET_Select_2_Switch_Status != shareMemSDK->DET_Select_2_Switch_Status) {
		ngxData.DET_Select_2_Switch_Status = shareMemSDK->DET_Select_2_Switch_Status;
		js["CARGO_DetSelect_2"] = shareMemSDK->DET_Select_2_Switch_Status;
	}
	//if (ngxData.CARGO_ArmedSw[0] != shareMemSDK->CARGO_ArmedSw[0]) {
	//	ngxData.CARGO_ArmedSw[0] = shareMemSDK->CARGO_ArmedSw[0];
	//	js["CARGO_ArmedSw_1"] = shareMemSDK->CARGO_ArmedSw[0] != 0;
	//}
	//if (ngxData.CARGO_ArmedSw[1] != shareMemSDK->CARGO_ArmedSw[1]) {
	//	ngxData.CARGO_ArmedSw[1] = shareMemSDK->CARGO_ArmedSw[1];
	//	js["CARGO_ArmedSw_2"] = shareMemSDK->CARGO_ArmedSw[1] != 0;
	//}
	if (ngxData.Engine_1_Start_Valve_Pos != shareMemSDK->Engine_1_Start_Valve_Pos) {
		ngxData.Engine_1_Start_Valve_Pos = shareMemSDK->Engine_1_Start_Valve_Pos;
		js["ENG_StartValve_1"] = shareMemSDK->Engine_1_Start_Valve_Pos != 0;
	}
	if (ngxData.Engine_2_Start_Valve_Pos != shareMemSDK->Engine_2_Start_Valve_Pos) {
		ngxData.Engine_2_Start_Valve_Pos = shareMemSDK->Engine_2_Start_Valve_Pos;
		js["ENG_StartValve_2"] = shareMemSDK->Engine_2_Start_Valve_Pos != 0;
	}
	//if (ngxData.COMM_SelectedMic[0] != shareMemSDK->COMM_SelectedMic[0]) {
	//	ngxData.COMM_SelectedMic[0] = shareMemSDK->COMM_SelectedMic[0];
	//	js["COMM_SelectedMic_1"] = shareMemSDK->COMM_SelectedMic[0] != 0;
	//}
	//if (ngxData.COMM_SelectedMic[1] != shareMemSDK->COMM_SelectedMic[1]) {
	//	ngxData.COMM_SelectedMic[1] = shareMemSDK->COMM_SelectedMic[1];
	//	js["COMM_SelectedMic_2"] = shareMemSDK->COMM_SelectedMic[1] != 0;
	//}
	//if (ngxData.COMM_SelectedMic[2] != shareMemSDK->COMM_SelectedMic[2]) {
	//	ngxData.COMM_SelectedMic[2] = shareMemSDK->COMM_SelectedMic[2];
	//	js["COMM_SelectedMic_3"] = shareMemSDK->COMM_SelectedMic[2] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[0] != shareMemSDK->COMM_ReceiverSwitches[0]) {
	//	ngxData.COMM_ReceiverSwitches[0] = shareMemSDK->COMM_ReceiverSwitches[0];
	//	js["COMM_ReceiverSwitches_1"] = shareMemSDK->COMM_ReceiverSwitches[0] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[1] != shareMemSDK->COMM_ReceiverSwitches[1]) {
	//	ngxData.COMM_ReceiverSwitches[1] = shareMemSDK->COMM_ReceiverSwitches[1];
	//	js["COMM_ReceiverSwitches_2"] = shareMemSDK->COMM_ReceiverSwitches[1] != 0;
	//}
	//if (ngxData.COMM_ReceiverSwitches[2] != shareMemSDK->COMM_ReceiverSwitches[2]) {
	//	ngxData.COMM_ReceiverSwitches[2] = shareMemSDK->COMM_ReceiverSwitches[2];
	//	js["COMM_ReceiverSwitches_3"] = shareMemSDK->COMM_ReceiverSwitches[2] != 0;
	//}


	if (ngxData.GPS_Light_Status != shareMemSDK->GPS_Light_Status) {
		ngxData.GPS_Light_Status = shareMemSDK->GPS_Light_Status;
		js["IRS_annunGPS"] = shareMemSDK->GPS_Light_Status != 0;
	}
	if (ngxData.PSEU_Light_Status != shareMemSDK->PSEU_Light_Status) {
		ngxData.PSEU_Light_Status = shareMemSDK->PSEU_Light_Status;
		js["WARN_annunPSEU"] = shareMemSDK->PSEU_Light_Status != 0;
	}
	if (ngxData.PASS_OXY_ON_Light_Status != shareMemSDK->PASS_OXY_ON_Light_Status) {
		ngxData.PASS_OXY_ON_Light_Status = shareMemSDK->PASS_OXY_ON_Light_Status;
		js["OXY_annunPASS_OXY_ON"] = shareMemSDK->PASS_OXY_ON_Light_Status != 0;
	}
	if (ngxData.LEFT_GEAR_GreenLight_Status != shareMemSDK->LEFT_GEAR_GreenLight_Status) {
		ngxData.LEFT_GEAR_GreenLight_Status = shareMemSDK->LEFT_GEAR_GreenLight_Status;
		js["GEAR_annunOvhdLEFT"] = shareMemSDK->LEFT_GEAR_GreenLight_Status != 0;
		js["MAIN_annunGEAR_locked_1"] = shareMemSDK->LEFT_GEAR_GreenLight_Status != 0;
	}
	if (ngxData.NOSE_GEAR_GreenLight_Status != shareMemSDK->NOSE_GEAR_GreenLight_Status) {
		ngxData.NOSE_GEAR_GreenLight_Status = shareMemSDK->NOSE_GEAR_GreenLight_Status;
		js["GEAR_annunOvhdNOSE"] = shareMemSDK->NOSE_GEAR_GreenLight_Status != 0;
		js["MAIN_annunGEAR_locked_2"] = shareMemSDK->NOSE_GEAR_GreenLight_Status != 0;
	}
	if (ngxData.RIGHT_GEAR_GreenLight_Status != shareMemSDK->RIGHT_GEAR_GreenLight_Status) {
		ngxData.RIGHT_GEAR_GreenLight_Status = shareMemSDK->RIGHT_GEAR_GreenLight_Status;
		js["GEAR_annunOvhdRIGHT"] = shareMemSDK->RIGHT_GEAR_GreenLight_Status != 0;
		js["MAIN_annunGEAR_locked_3"] = shareMemSDK->RIGHT_GEAR_GreenLight_Status != 0;
	}
	//if (ngxData.FLTREC_annunOFF != shareMemSDK->FLTREC_annunOFF) {
	//	ngxData.FLTREC_annunOFF = shareMemSDK->FLTREC_annunOFF;
	//	js["FLTREC_annunOFF"] = shareMemSDK->FLTREC_annunOFF != 0;
	//}
	if (ngxData.YAW_DAMPER_Light_Status != shareMemSDK->YAW_DAMPER_Light_Status) {
		ngxData.YAW_DAMPER_Light_Status = shareMemSDK->YAW_DAMPER_Light_Status;
		js["FCTL_annunYAW_DAMPER"] = shareMemSDK->YAW_DAMPER_Light_Status != 0;
	}
	if (ngxData.LOW_QUANTITY_Light_Status != shareMemSDK->LOW_QUANTITY_Light_Status) {
		ngxData.LOW_QUANTITY_Light_Status = shareMemSDK->LOW_QUANTITY_Light_Status;
		js["FCTL_annunLOW_QUANTITY"] = shareMemSDK->LOW_QUANTITY_Light_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_BACKUP_Light_Status != shareMemSDK->LOW_PRESSURE_BACKUP_Light_Status) {
		ngxData.LOW_PRESSURE_BACKUP_Light_Status = shareMemSDK->LOW_PRESSURE_BACKUP_Light_Status;
		js["FCTL_annunLOW_PRESSURE"] = shareMemSDK->LOW_PRESSURE_BACKUP_Light_Status != 0;
	}
	if (ngxData.STBY_RUD_ON_Light_Status != shareMemSDK->STBY_RUD_ON_Light_Status) {
		ngxData.STBY_RUD_ON_Light_Status = shareMemSDK->STBY_RUD_ON_Light_Status;
		js["FCTL_annunLOW_STBY_RUD_ON"] = shareMemSDK->STBY_RUD_ON_Light_Status != 0;
	}
	if (ngxData.FEEL_DIFF_PRESS_Light_Status != shareMemSDK->FEEL_DIFF_PRESS_Light_Status) {
		ngxData.FEEL_DIFF_PRESS_Light_Status = shareMemSDK->FEEL_DIFF_PRESS_Light_Status;
		js["FCTL_annunFEEL_DIFF_PRESS"] = shareMemSDK->FEEL_DIFF_PRESS_Light_Status != 0;
	}
	if (ngxData.SPEED_TRIM_FAIL_Light_Status != shareMemSDK->SPEED_TRIM_FAIL_Light_Status) {
		ngxData.SPEED_TRIM_FAIL_Light_Status = shareMemSDK->SPEED_TRIM_FAIL_Light_Status;
		js["FCTL_annunSPEED_TRIM_FAIL"] = shareMemSDK->SPEED_TRIM_FAIL_Light_Status != 0;
	}
	if (ngxData.MACH_TRIM_FAIL_Light_Status != shareMemSDK->MACH_TRIM_FAIL_Light_Status) {
		ngxData.MACH_TRIM_FAIL_Light_Status = shareMemSDK->MACH_TRIM_FAIL_Light_Status;
		js["FCTL_annunMACH_TRIM_FAIL"] = shareMemSDK->MACH_TRIM_FAIL_Light_Status != 0;
	}
	if (ngxData.AUTO_SLAT_FAIL_Light_Status != shareMemSDK->AUTO_SLAT_FAIL_Light_Status) {
		ngxData.AUTO_SLAT_FAIL_Light_Status = shareMemSDK->AUTO_SLAT_FAIL_Light_Status;
		js["FCTL_annunAUTO_SLAT_FAIL"] = shareMemSDK->AUTO_SLAT_FAIL_Light_Status != 0;
	}
	if (ngxData.VALVE_OPEN_Light_Status != shareMemSDK->VALVE_OPEN_Light_Status) {
		ngxData.VALVE_OPEN_Light_Status = shareMemSDK->VALVE_OPEN_Light_Status;
		js["FUEL_annunXFEED_VALVE_OPEN"] = shareMemSDK->VALVE_OPEN_Light_Status;
	}
	if (ngxData.BAT_DISCHARGE_Light_Status != shareMemSDK->BAT_DISCHARGE_Light_Status) {
		ngxData.BAT_DISCHARGE_Light_Status = shareMemSDK->BAT_DISCHARGE_Light_Status;
		js["ELEC_annunBAT_DISCHARGE"] = shareMemSDK->BAT_DISCHARGE_Light_Status != 0;
	}
	if (ngxData.TR_UNIT_Light_Status != shareMemSDK->TR_UNIT_Light_Status) {
		ngxData.TR_UNIT_Light_Status = shareMemSDK->TR_UNIT_Light_Status;
		js["ELEC_annunTR_UNIT"] = shareMemSDK->TR_UNIT_Light_Status != 0;
	}
	if (ngxData.ELEC_Light_Status != shareMemSDK->ELEC_Light_Status) {
		ngxData.ELEC_Light_Status = shareMemSDK->ELEC_Light_Status;
		js["ELEC_annunELEC"] = shareMemSDK->ELEC_Light_Status != 0;
	}
	if (ngxData.STANDBY_PWR_OFF_Light_Status != shareMemSDK->STANDBY_PWR_OFF_Light_Status) {
		ngxData.STANDBY_PWR_OFF_Light_Status = shareMemSDK->STANDBY_PWR_OFF_Light_Status;
		js["ELEC_annunSTANDBY_POWER_OFF"] = shareMemSDK->STANDBY_PWR_OFF_Light_Status != 0;
	}
	if (ngxData.GRD_POWER_AVAILABLE_Light_Status != shareMemSDK->GRD_POWER_AVAILABLE_Light_Status) {
		ngxData.GRD_POWER_AVAILABLE_Light_Status = shareMemSDK->GRD_POWER_AVAILABLE_Light_Status;
		js["ELEC_annunGRD_POWER_AVAILABLE"] = shareMemSDK->GRD_POWER_AVAILABLE_Light_Status != 0;
	}
	if (ngxData.APU_GEN_OFF_BUS_Light_Status != shareMemSDK->APU_GEN_OFF_BUS_Light_Status) {
		ngxData.APU_GEN_OFF_BUS_Light_Status = shareMemSDK->APU_GEN_OFF_BUS_Light_Status;
		js["ELEC_annunAPU_GEN_OFF_BUS"] = shareMemSDK->APU_GEN_OFF_BUS_Light_Status != 0;
	}
	//if (ngxData.APU_annunMAINT != shareMemSDK->APU_annunMAINT) {
	//	ngxData.APU_annunMAINT = shareMemSDK->APU_annunMAINT;
	//	js["APU_annunMAINT"] = shareMemSDK->APU_annunMAINT != 0;
	//}
	//if (ngxData.APU_annunLOW_OIL_PRESSURE != shareMemSDK->APU_annunLOW_OIL_PRESSURE) {
	//	ngxData.APU_annunLOW_OIL_PRESSURE = shareMemSDK->APU_annunLOW_OIL_PRESSURE;
	//	js["APU_annunLOW_OIL_PRESSURE"] = shareMemSDK->APU_annunLOW_OIL_PRESSURE != 0;
	//}
	//if (ngxData.APU_annunFAULT != shareMemSDK->APU_annunFAULT) {
	//	ngxData.APU_annunFAULT = shareMemSDK->APU_annunFAULT;
	//	js["APU_annunFAULT"] = shareMemSDK->APU_annunFAULT != 0;
	//}
	//if (ngxData.APU_annunOVERSPEED != shareMemSDK->APU_annunOVERSPEED) {
	//	ngxData.APU_annunOVERSPEED = shareMemSDK->APU_annunOVERSPEED;
	//	js["APU_annunOVERSPEED"] = shareMemSDK->APU_annunOVERSPEED != 0;
	//}
	if (ngxData.Equip_Cooling_OFF_Light_1_Status != shareMemSDK->Equip_Cooling_OFF_Light_1_Status) {
		ngxData.Equip_Cooling_OFF_Light_1_Status = shareMemSDK->Equip_Cooling_OFF_Light_1_Status;
		js["AIR_annunEquipCoolingSupplyOFF"] = shareMemSDK->Equip_Cooling_OFF_Light_1_Status != 0;
	}
	if (ngxData.Equip_Cooling_OFF_Light_2_Status != shareMemSDK->Equip_Cooling_OFF_Light_2_Status) {
		ngxData.Equip_Cooling_OFF_Light_2_Status = shareMemSDK->Equip_Cooling_OFF_Light_2_Status;
		js["AIR_annunEquipCoolingExhaustOFF"] = shareMemSDK->Equip_Cooling_OFF_Light_2_Status != 0;
	}
	if (ngxData.Emergency_Light_Status != shareMemSDK->Emergency_Light_Status) {
		ngxData.Emergency_Light_Status = shareMemSDK->Emergency_Light_Status;
		js["LTS_annunEmerNOT_ARMED"] = shareMemSDK->Emergency_Light_Status != 0;
	}
	//if (ngxData.COMM_annunCALL != shareMemSDK->COMM_annunCALL) {
	//	ngxData.COMM_annunCALL = shareMemSDK->COMM_annunCALL;
	//	js["COMM_annunCALL"] = shareMemSDK->COMM_annunCALL != 0;
	//}
	//if (ngxData.COMM_annunPA_IN_USE != shareMemSDK->COMM_annunPA_IN_USE) {
	//	ngxData.COMM_annunPA_IN_USE = shareMemSDK->COMM_annunPA_IN_USE;
	//	js["COMM_annunPA_IN_USE"] = shareMemSDK->COMM_annunPA_IN_USE != 0;
	//}
	if (ngxData.CAPT_PITOT_Lights_Status != shareMemSDK->CAPT_PITOT_Lights_Status) {
		ngxData.CAPT_PITOT_Lights_Status = shareMemSDK->CAPT_PITOT_Lights_Status;
		js["ICE_annunCAPT_PITOT"] = shareMemSDK->CAPT_PITOT_Lights_Status != 0;
	}
	if (ngxData.L_ELEV_PITOT_Lights_Status != shareMemSDK->L_ELEV_PITOT_Lights_Status) {
		ngxData.L_ELEV_PITOT_Lights_Status = shareMemSDK->L_ELEV_PITOT_Lights_Status;
		js["ICE_annunL_ELEV_PITOT"] = shareMemSDK->L_ELEV_PITOT_Lights_Status != 0;
	}
	if (ngxData.L_ALPHA_VANE_Lights_Status != shareMemSDK->L_ALPHA_VANE_Lights_Status) {
		ngxData.L_ALPHA_VANE_Lights_Status = shareMemSDK->L_ALPHA_VANE_Lights_Status;
		js["ICE_annunL_ALPHA_VANE"] = shareMemSDK->L_ALPHA_VANE_Lights_Status != 0;
	}
	if (ngxData.TEMP_PROBE_Lights_Status != shareMemSDK->TEMP_PROBE_Lights_Status) {
		ngxData.TEMP_PROBE_Lights_Status = shareMemSDK->TEMP_PROBE_Lights_Status;
		js["ICE_annunL_TEMP_PROBE"] = shareMemSDK->TEMP_PROBE_Lights_Status != 0;
	}
	if (ngxData.FO_PITOT_Lights_Status != shareMemSDK->FO_PITOT_Lights_Status) {
		ngxData.FO_PITOT_Lights_Status = shareMemSDK->FO_PITOT_Lights_Status;
		js["ICE_annunFO_PITOT"] = shareMemSDK->FO_PITOT_Lights_Status != 0;
	}
	if (ngxData.R_ELEV_PITOT_Lights_Status != shareMemSDK->R_ELEV_PITOT_Lights_Status) {
		ngxData.R_ELEV_PITOT_Lights_Status = shareMemSDK->R_ELEV_PITOT_Lights_Status;
		js["ICE_annunR_ELEV_PITOT"] = shareMemSDK->R_ELEV_PITOT_Lights_Status != 0;
	}
	if (ngxData.R_ALPHA_VANE_Lights_Status != shareMemSDK->R_ALPHA_VANE_Lights_Status) {
		ngxData.R_ALPHA_VANE_Lights_Status = shareMemSDK->R_ALPHA_VANE_Lights_Status;
		js["ICE_annunR_ALPHA_VANE"] = shareMemSDK->R_ALPHA_VANE_Lights_Status != 0;
	}
	if (ngxData.AUX_PITOT_Lights_Status != shareMemSDK->AUX_PITOT_Lights_Status) {
		ngxData.AUX_PITOT_Lights_Status = shareMemSDK->AUX_PITOT_Lights_Status;
		js["ICE_annunAUX_PITOT"] = shareMemSDK->AUX_PITOT_Lights_Status != 0;
	}
	if (ngxData.DUAL_BLEED_Light_Status != shareMemSDK->DUAL_BLEED_Light_Status) {
		ngxData.DUAL_BLEED_Light_Status = shareMemSDK->DUAL_BLEED_Light_Status;
		js["AIR_annunDualBleed"] = shareMemSDK->DUAL_BLEED_Light_Status != 0;
	}
	if (ngxData.RAM_DOOR_FUEL_DOOR_1_Light_Status != shareMemSDK->RAM_DOOR_FUEL_DOOR_1_Light_Status) {
		ngxData.RAM_DOOR_FUEL_DOOR_1_Light_Status = shareMemSDK->RAM_DOOR_FUEL_DOOR_1_Light_Status;
		js["AIR_annunRamDoorL"] = shareMemSDK->RAM_DOOR_FUEL_DOOR_1_Light_Status != 0;
	}
	if (ngxData.RAM_DOOR_FUEL_DOOR_2_Light_Status != shareMemSDK->RAM_DOOR_FUEL_DOOR_2_Light_Status) {
		ngxData.RAM_DOOR_FUEL_DOOR_2_Light_Status = shareMemSDK->RAM_DOOR_FUEL_DOOR_2_Light_Status;
		js["AIR_annunRamDoorR"] = shareMemSDK->RAM_DOOR_FUEL_DOOR_2_Light_Status != 0;
	}
	//if (ngxData.AIR_FltAltWindow != shareMemSDK->AIR_FltAltWindow) {
	//	ngxData.AIR_FltAltWindow = shareMemSDK->AIR_FltAltWindow;
	//	js["AIR_FltAltWindow"] = shareMemSDK->AIR_FltAltWindow != 0;
	//}
	//if (ngxData.AIR_LandAltWindow != shareMemSDK->AIR_LandAltWindow) {
	//	ngxData.AIR_LandAltWindow = shareMemSDK->AIR_LandAltWindow;
	//	js["AIR_LandAltWindow"] = shareMemSDK->AIR_LandAltWindow != 0;
	//}
	if (ngxData.Warning_FLT_CONT_Light_Status != shareMemSDK->Warning_FLT_CONT_Light_Status) {
		ngxData.Warning_FLT_CONT_Light_Status = shareMemSDK->Warning_FLT_CONT_Light_Status;
		js["WARN_annunFLT_CONT"] = shareMemSDK->Warning_FLT_CONT_Light_Status != 0;
	}
	if (ngxData.Warning_IRS_Light_Status != shareMemSDK->Warning_IRS_Light_Status) {
		ngxData.Warning_IRS_Light_Status = shareMemSDK->Warning_IRS_Light_Status;
		js["WARN_annunIRS"] = shareMemSDK->Warning_IRS_Light_Status != 0;
	}
	if (ngxData.Warning_FUEL_Light_Status != shareMemSDK->Warning_FUEL_Light_Status) {
		ngxData.Warning_FUEL_Light_Status = shareMemSDK->Warning_FUEL_Light_Status;
		js["WARN_annunFUEL"] = shareMemSDK->Warning_FUEL_Light_Status != 0;
	}
	if (ngxData.Warning_ELEC_Light_Status != shareMemSDK->Warning_ELEC_Light_Status) {
		ngxData.Warning_ELEC_Light_Status = shareMemSDK->Warning_ELEC_Light_Status;
		js["WARN_annunELEC"] = shareMemSDK->Warning_ELEC_Light_Status != 0;
	}
	if (ngxData.Warning_APU_Light_Status != shareMemSDK->Warning_APU_Light_Status) {
		ngxData.Warning_APU_Light_Status = shareMemSDK->Warning_APU_Light_Status;
		js["WARN_annunAPU"] = shareMemSDK->Warning_APU_Light_Status != 0;
	}
	if (ngxData.Warning_OVHT_Light_Status != shareMemSDK->Warning_OVHT_Light_Status) {
		ngxData.Warning_OVHT_Light_Status = shareMemSDK->Warning_OVHT_Light_Status;
		js["WARN_annunOVHT_DET"] = shareMemSDK->Warning_OVHT_Light_Status != 0;
	}
	if (ngxData.Warning_ANTIICE_Light_Status != shareMemSDK->Warning_ANTIICE_Light_Status) {
		ngxData.Warning_ANTIICE_Light_Status = shareMemSDK->Warning_ANTIICE_Light_Status;
		js["WARN_annunANTI_ICE"] = shareMemSDK->Warning_ANTIICE_Light_Status != 0;
	}
	if (ngxData.Warning_HYD_Light_Status != shareMemSDK->Warning_HYD_Light_Status) {
		ngxData.Warning_HYD_Light_Status = shareMemSDK->Warning_HYD_Light_Status;
		js["WARN_annunHYD"] = shareMemSDK->Warning_HYD_Light_Status != 0;
	}
	if (ngxData.Warning_DOORS_Light_Status != shareMemSDK->Warning_DOORS_Light_Status) {
		ngxData.Warning_DOORS_Light_Status = shareMemSDK->Warning_DOORS_Light_Status;
		js["WARN_annunDOORS"] = shareMemSDK->Warning_DOORS_Light_Status != 0;
	}
	if (ngxData.Warning_ENG_Light_Status != shareMemSDK->Warning_ENG_Light_Status) {
		ngxData.Warning_ENG_Light_Status = shareMemSDK->Warning_ENG_Light_Status;
		js["WARN_annunENG"] = shareMemSDK->Warning_ENG_Light_Status != 0;
	}
	if (ngxData.Warning_OVERHEAD_Light_Status != shareMemSDK->Warning_OVERHEAD_Light_Status) {
		ngxData.Warning_OVERHEAD_Light_Status = shareMemSDK->Warning_OVERHEAD_Light_Status;
		js["WARN_annunOVERHEAD"] = shareMemSDK->Warning_OVERHEAD_Light_Status != 0;
	}
	if (ngxData.Warning_AIR_COND_Light_Status != shareMemSDK->Warning_AIR_COND_Light_Status) {
		ngxData.Warning_AIR_COND_Light_Status = shareMemSDK->Warning_AIR_COND_Light_Status;
		js["WARN_annunAIR_COND"] = shareMemSDK->Warning_AIR_COND_Light_Status != 0;
	}
	if (ngxData.SPEED_BRAKE_ARMED_Light_Status != shareMemSDK->SPEED_BRAKE_ARMED_Light_Status) {
		ngxData.SPEED_BRAKE_ARMED_Light_Status = shareMemSDK->SPEED_BRAKE_ARMED_Light_Status;
		js["MAIN_annunSPEEDBRAKE_ARMED"] = shareMemSDK->SPEED_BRAKE_ARMED_Light_Status != 0;
	}
	if (ngxData.SPEED_BRAKE_DO_NOT_ARM_Light_Status != shareMemSDK->SPEED_BRAKE_DO_NOT_ARM_Light_Status) {
		ngxData.SPEED_BRAKE_DO_NOT_ARM_Light_Status = shareMemSDK->SPEED_BRAKE_DO_NOT_ARM_Light_Status;
		js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = shareMemSDK->SPEED_BRAKE_DO_NOT_ARM_Light_Status != 0;
	}
	if (ngxData.SPEEDBRAKES_EXTENDED_Light_Status != shareMemSDK->SPEEDBRAKES_EXTENDED_Light_Status) {
		ngxData.SPEEDBRAKES_EXTENDED_Light_Status = shareMemSDK->SPEEDBRAKES_EXTENDED_Light_Status;
		js["MAIN_annunSPEEDBRAKE_EXTENDED"] = shareMemSDK->SPEEDBRAKES_EXTENDED_Light_Status != 0;
	}
	//if (ngxData.MAIN_annunSTAB_OUT_OF_TRIM != shareMemSDK->MAIN_annunSTAB_OUT_OF_TRIM) {
	//	ngxData.MAIN_annunSTAB_OUT_OF_TRIM = shareMemSDK->MAIN_annunSTAB_OUT_OF_TRIM;
	//	js["MAIN_annunSTAB_OUT_OF_TRIM"] = shareMemSDK->MAIN_annunSTAB_OUT_OF_TRIM != 0;
	//}
	if (ngxData.ANTISKID_INOP_Light_Status != shareMemSDK->ANTISKID_INOP_Light_Status) {
		ngxData.ANTISKID_INOP_Light_Status = shareMemSDK->ANTISKID_INOP_Light_Status;
		js["MAIN_annunANTI_SKID_INOP"] = shareMemSDK->ANTISKID_INOP_Light_Status != 0;
	}
	if (ngxData.AUTO_BRAKE_DISARM_Light_Status != shareMemSDK->AUTO_BRAKE_DISARM_Light_Status) {
		ngxData.AUTO_BRAKE_DISARM_Light_Status = shareMemSDK->AUTO_BRAKE_DISARM_Light_Status;
		js["MAIN_annunAUTO_BRAKE_DISARM"] = shareMemSDK->AUTO_BRAKE_DISARM_Light_Status != 0;
	}
	//if (ngxData.MAIN_annunLE_FLAPS_TRANSIT != shareMemSDK->MAIN_annunLE_FLAPS_TRANSIT) {
	//	ngxData.MAIN_annunLE_FLAPS_TRANSIT = shareMemSDK->MAIN_annunLE_FLAPS_TRANSIT;
	//	js["MAIN_annunLE_FLAPS_TRANSIT"] = shareMemSDK->MAIN_annunLE_FLAPS_TRANSIT != 0;
	//}
	//if (ngxData.MAIN_annunLE_FLAPS_EXT != shareMemSDK->MAIN_annunLE_FLAPS_EXT) {
	//	ngxData.MAIN_annunLE_FLAPS_EXT = shareMemSDK->MAIN_annunLE_FLAPS_EXT;
	//	js["MAIN_annunLE_FLAPS_EXT"] = shareMemSDK->MAIN_annunLE_FLAPS_EXT != 0;
	//}
	//if (ngxData.HGS_annun_AIII != shareMemSDK->HGS_annun_AIII) {
	//	ngxData.HGS_annun_AIII = shareMemSDK->HGS_annun_AIII;
	//	js["HGS_annun_AIII"] = shareMemSDK->HGS_annun_AIII != 0;
	//}
	//if (ngxData.HGS_annun_NO_AIII != shareMemSDK->HGS_annun_NO_AIII) {
	//	ngxData.HGS_annun_NO_AIII = shareMemSDK->HGS_annun_NO_AIII;
	//	js["HGS_annun_NO_AIII"] = shareMemSDK->HGS_annun_NO_AIII != 0;
	//}
	//if (ngxData.HGS_annun_FLARE != shareMemSDK->HGS_annun_FLARE) {
	//	ngxData.HGS_annun_FLARE = shareMemSDK->HGS_annun_FLARE;
	//	js["HGS_annun_FLARE"] = shareMemSDK->HGS_annun_FLARE != 0;
	//}
	//if (ngxData.HGS_annun_RO != shareMemSDK->HGS_annun_RO) {
	//	ngxData.HGS_annun_RO = shareMemSDK->HGS_annun_RO;
	//	js["HGS_annun_RO"] = shareMemSDK->HGS_annun_RO != 0;
	//}
	//if (ngxData.HGS_annun_RO_CTN != shareMemSDK->HGS_annun_RO_CTN) {
	//	ngxData.HGS_annun_RO_CTN = shareMemSDK->HGS_annun_RO_CTN;
	//	js["HGS_annun_RO_CTN"] = shareMemSDK->HGS_annun_RO_CTN != 0;
	//}
	//if (ngxData.HGS_annun_RO_ARM != shareMemSDK->HGS_annun_RO_ARM) {
	//	ngxData.HGS_annun_RO_ARM = shareMemSDK->HGS_annun_RO_ARM;
	//	js["HGS_annun_RO_ARM"] = shareMemSDK->HGS_annun_RO_ARM != 0;
	//}
	//if (ngxData.HGS_annun_TO != shareMemSDK->HGS_annun_TO) {
	//	ngxData.HGS_annun_TO = shareMemSDK->HGS_annun_TO;
	//	js["HGS_annun_TO"] = shareMemSDK->HGS_annun_TO != 0;
	//}
	//if (ngxData.HGS_annun_TO_CTN != shareMemSDK->HGS_annun_TO_CTN) {
	//	ngxData.HGS_annun_TO_CTN = shareMemSDK->HGS_annun_TO_CTN;
	//	js["HGS_annun_TO_CTN"] = shareMemSDK->HGS_annun_TO_CTN != 0;
	//}
	//if (ngxData.HGS_annun_APCH != shareMemSDK->HGS_annun_APCH) {
	//	ngxData.HGS_annun_APCH = shareMemSDK->HGS_annun_APCH;
	//	js["HGS_annun_APCH"] = shareMemSDK->HGS_annun_APCH != 0;
	//}
	//if (ngxData.HGS_annun_TO_WARN != shareMemSDK->HGS_annun_TO_WARN) {
	//	ngxData.HGS_annun_TO_WARN = shareMemSDK->HGS_annun_TO_WARN;
	//	js["HGS_annun_TO_WARN"] = shareMemSDK->HGS_annun_TO_WARN != 0;
	//}
	//if (ngxData.HGS_annun_Bar != shareMemSDK->HGS_annun_Bar) {
	//	ngxData.HGS_annun_Bar = shareMemSDK->HGS_annun_Bar;
	//	js["HGS_annun_Bar"] = shareMemSDK->HGS_annun_Bar != 0;
	//}
	//if (ngxData.HGS_annun_FAIL != shareMemSDK->HGS_annun_FAIL) {
	//	ngxData.HGS_annun_FAIL = shareMemSDK->HGS_annun_FAIL;
	//	js["HGS_annun_FAIL"] = shareMemSDK->HGS_annun_FAIL != 0;
	//}
	//if (ngxData.GPWS_annunINOP != shareMemSDK->GPWS_annunINOP) {
	//	ngxData.GPWS_annunINOP = shareMemSDK->GPWS_annunINOP;
	//	js["GPWS_annunINOP"] = shareMemSDK->GPWS_annunINOP != 0;
	//}
	if (ngxData.Parking_Brake_Light_Status != shareMemSDK->Parking_Brake_Light_Status) {
		ngxData.Parking_Brake_Light_Status = shareMemSDK->Parking_Brake_Light_Status;
		js["PED_annunParkingBrake"] = shareMemSDK->Parking_Brake_Light_Status != 0;
	}
	//if (ngxData.FIRE_annunWHEEL_WELL != shareMemSDK->FIRE_annunWHEEL_WELL) {
	//	ngxData.FIRE_annunWHEEL_WELL = shareMemSDK->FIRE_annunWHEEL_WELL;
	//	js["FIRE_annunWHEEL_WELL"] = shareMemSDK->FIRE_annunWHEEL_WELL != 0;
	//}
	//if (ngxData.FIRE_annunFAULT != shareMemSDK->FIRE_annunFAULT) {
	//	ngxData.FIRE_annunFAULT = shareMemSDK->FIRE_annunFAULT;
	//	js["FIRE_annunFAULT"] = shareMemSDK->FIRE_annunFAULT != 0;
	//}
	//if (ngxData.FIRE_annunAPU_DET_INOP != shareMemSDK->FIRE_annunAPU_DET_INOP) {
	//	ngxData.FIRE_annunAPU_DET_INOP = shareMemSDK->FIRE_annunAPU_DET_INOP;
	//	js["FIRE_annunAPU_DET_INOP"] = shareMemSDK->FIRE_annunAPU_DET_INOP != 0;
	//}
	//if (ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE != shareMemSDK->FIRE_annunAPU_BOTTLE_DISCHARGE) {
	//	ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE = shareMemSDK->FIRE_annunAPU_BOTTLE_DISCHARGE;
	//	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = shareMemSDK->FIRE_annunAPU_BOTTLE_DISCHARGE != 0;
	//}
	//if (ngxData.CARGO_annunFWD != shareMemSDK->CARGO_annunFWD) {
	//	ngxData.CARGO_annunFWD = shareMemSDK->CARGO_annunFWD;
	//	js["CARGO_annunFWD"] = shareMemSDK->CARGO_annunFWD != 0;
	//}
	//if (ngxData.CARGO_annunAFT != shareMemSDK->CARGO_annunAFT) {
	//	ngxData.CARGO_annunAFT = shareMemSDK->CARGO_annunAFT;
	//	js["CARGO_annunAFT"] = shareMemSDK->CARGO_annunAFT != 0;
	//}
	//if (ngxData.CARGO_annunDETECTOR_FAULT != shareMemSDK->CARGO_annunDETECTOR_FAULT) {
	//	ngxData.CARGO_annunDETECTOR_FAULT = shareMemSDK->CARGO_annunDETECTOR_FAULT;
	//	js["CARGO_annunDETECTOR_FAULT"] = shareMemSDK->CARGO_annunDETECTOR_FAULT != 0;
	//}
	//if (ngxData.CARGO_annunDISCH != shareMemSDK->CARGO_annunDISCH) {
	//	ngxData.CARGO_annunDISCH = shareMemSDK->CARGO_annunDISCH;
	//	js["CARGO_annunDISCH"] = shareMemSDK->CARGO_annunDISCH != 0;
	//}
	//if (ngxData.HGS_annunRWY != shareMemSDK->HGS_annunRWY) {
	//	ngxData.HGS_annunRWY = shareMemSDK->HGS_annunRWY;
	//	js["HGS_annunRWY"] = shareMemSDK->HGS_annunRWY != 0;
	//}
	//if (ngxData.HGS_annunGS != shareMemSDK->HGS_annunGS) {
	//	ngxData.HGS_annunGS = shareMemSDK->HGS_annunGS;
	//	js["HGS_annunGS"] = shareMemSDK->HGS_annunGS != 0;
	//}
	//if (ngxData.HGS_annunFAULT != shareMemSDK->HGS_annunFAULT) {
	//	ngxData.HGS_annunFAULT = shareMemSDK->HGS_annunFAULT;
	//	js["HGS_annunFAULT"] = shareMemSDK->HGS_annunFAULT != 0;
	//}
	//if (ngxData.HGS_annunCLR != shareMemSDK->HGS_annunCLR) {
	//	ngxData.HGS_annunCLR = shareMemSDK->HGS_annunCLR;
	//	js["HGS_annunCLR"] = shareMemSDK->HGS_annunCLR != 0;
	//}
	//if (ngxData.XPDR_annunFAIL != shareMemSDK->XPDR_annunFAIL) {
	//	ngxData.XPDR_annunFAIL = shareMemSDK->XPDR_annunFAIL;
	//	js["XPDR_annunFAIL"] = shareMemSDK->XPDR_annunFAIL != 0;
	//}
	//if (ngxData.PED_annunLOCK_FAIL != shareMemSDK->PED_annunLOCK_FAIL) {
	//	ngxData.PED_annunLOCK_FAIL = shareMemSDK->PED_annunLOCK_FAIL;
	//	js["PED_annunLOCK_FAIL"] = shareMemSDK->PED_annunLOCK_FAIL != 0;
	//}
	//if (ngxData.PED_annunAUTO_UNLK != shareMemSDK->PED_annunAUTO_UNLK) {
	//	ngxData.PED_annunAUTO_UNLK = shareMemSDK->PED_annunAUTO_UNLK;
	//	js["PED_annunAUTO_UNLK"] = shareMemSDK->PED_annunAUTO_UNLK != 0;
	//}
	//if (ngxData.DOOR_annunFWD_ENTRY != shareMemSDK->DOOR_annunFWD_ENTRY) {
	//	ngxData.DOOR_annunFWD_ENTRY = shareMemSDK->DOOR_annunFWD_ENTRY;
	//	js["DOOR_annunFWD_ENTRY"] = shareMemSDK->DOOR_annunFWD_ENTRY != 0;
	//}
	//if (ngxData.DOOR_annunFWD_SERVICE != shareMemSDK->DOOR_annunFWD_SERVICE) {
	//	ngxData.DOOR_annunFWD_SERVICE = shareMemSDK->DOOR_annunFWD_SERVICE;
	//	js["DOOR_annunFWD_SERVICE"] = shareMemSDK->DOOR_annunFWD_SERVICE != 0;
	//}
	//if (ngxData.DOOR_annunAIRSTAIR != shareMemSDK->DOOR_annunAIRSTAIR) {
	//	ngxData.DOOR_annunAIRSTAIR = shareMemSDK->DOOR_annunAIRSTAIR;
	//	js["DOOR_annunAIRSTAIR"] = shareMemSDK->DOOR_annunAIRSTAIR != 0;
	//}
	//if (ngxData.DOOR_annunLEFT_FWD_OVERWING != shareMemSDK->DOOR_annunLEFT_FWD_OVERWING) {
	//	ngxData.DOOR_annunLEFT_FWD_OVERWING = shareMemSDK->DOOR_annunLEFT_FWD_OVERWING;
	//	js["DOOR_annunLEFT_FWD_OVERWING"] = shareMemSDK->DOOR_annunLEFT_FWD_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunRIGHT_FWD_OVERWING != shareMemSDK->DOOR_annunRIGHT_FWD_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_FWD_OVERWING = shareMemSDK->DOOR_annunRIGHT_FWD_OVERWING;
	//	js["DOOR_annunRIGHT_FWD_OVERWING"] = shareMemSDK->DOOR_annunRIGHT_FWD_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunFWD_CARGO != shareMemSDK->DOOR_annunFWD_CARGO) {
	//	ngxData.DOOR_annunFWD_CARGO = shareMemSDK->DOOR_annunFWD_CARGO;
	//	js["DOOR_annunFWD_CARGO"] = shareMemSDK->DOOR_annunFWD_CARGO != 0;
	//}
	//if (ngxData.DOOR_annunEQUIP != shareMemSDK->DOOR_annunEQUIP) {
	//	ngxData.DOOR_annunEQUIP = shareMemSDK->DOOR_annunEQUIP;
	//	js["DOOR_annunEQUIP"] = shareMemSDK->DOOR_annunEQUIP != 0;
	//}
	//if (ngxData.DOOR_annunLEFT_AFT_OVERWING != shareMemSDK->DOOR_annunLEFT_AFT_OVERWING) {
	//	ngxData.DOOR_annunLEFT_AFT_OVERWING = shareMemSDK->DOOR_annunLEFT_AFT_OVERWING;
	//	js["DOOR_annunLEFT_AFT_OVERWING"] = shareMemSDK->DOOR_annunLEFT_AFT_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunRIGHT_AFT_OVERWING != shareMemSDK->DOOR_annunRIGHT_AFT_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_AFT_OVERWING = shareMemSDK->DOOR_annunRIGHT_AFT_OVERWING;
	//	js["DOOR_annunRIGHT_AFT_OVERWING"] = shareMemSDK->DOOR_annunRIGHT_AFT_OVERWING != 0;
	//}
	//if (ngxData.DOOR_annunAFT_CARGO != shareMemSDK->DOOR_annunAFT_CARGO) {
	//	ngxData.DOOR_annunAFT_CARGO = shareMemSDK->DOOR_annunAFT_CARGO;
	//	js["DOOR_annunAFT_CARGO"] = shareMemSDK->DOOR_annunAFT_CARGO != 0;
	//}
	//if (ngxData.DOOR_annunAFT_ENTRY != shareMemSDK->DOOR_annunAFT_ENTRY) {
	//	ngxData.DOOR_annunAFT_ENTRY = shareMemSDK->DOOR_annunAFT_ENTRY;
	//	js["DOOR_annunAFT_ENTRY"] = shareMemSDK->DOOR_annunAFT_ENTRY != 0;
	//}
	//if (ngxData.DOOR_annunAFT_SERVICE != shareMemSDK->DOOR_annunAFT_SERVICE) {
	//	ngxData.DOOR_annunAFT_SERVICE = shareMemSDK->DOOR_annunAFT_SERVICE;
	//	js["DOOR_annunAFT_SERVICE"] = shareMemSDK->DOOR_annunAFT_SERVICE != 0;
	//}
	if (ngxData.AUTO_FAIL_Light_Status != shareMemSDK->AUTO_FAIL_Light_Status) {
		ngxData.AUTO_FAIL_Light_Status = shareMemSDK->AUTO_FAIL_Light_Status;
		js["AIR_annunAUTO_FAIL"] = shareMemSDK->AUTO_FAIL_Light_Status != 0;
	}
	if (ngxData.OFF_SCHED_DESCENT_Light_Status != shareMemSDK->OFF_SCHED_DESCENT_Light_Status) {
		ngxData.OFF_SCHED_DESCENT_Light_Status = shareMemSDK->OFF_SCHED_DESCENT_Light_Status;
		js["AIR_annunOFFSCHED_DESCENT"] = shareMemSDK->OFF_SCHED_DESCENT_Light_Status != 0;
	}
	if (ngxData.ALTN_Light_Status != shareMemSDK->ALTN_Light_Status) {
		ngxData.ALTN_Light_Status = shareMemSDK->ALTN_Light_Status;
		js["AIR_annunALTN"] = shareMemSDK->ALTN_Light_Status != 0;
	}
	if (ngxData.MANUAL_Light_Status != shareMemSDK->MANUAL_Light_Status) {
		ngxData.MANUAL_Light_Status = shareMemSDK->MANUAL_Light_Status;
		js["AIR_annunMANUAL"] = shareMemSDK->MANUAL_Light_Status != 0;
	}



	//if (ngxData.IRS_annunALIGN[0] != shareMemSDK->IRS_annunALIGN[0]) {
	//	ngxData.IRS_annunALIGN[0] = shareMemSDK->IRS_annunALIGN[0];
	//	js["IRS_annunALIGN_1"] = shareMemSDK->IRS_annunALIGN[0] != 0;
	//}
	//if (ngxData.IRS_annunALIGN[1] != shareMemSDK->IRS_annunALIGN[1]) {
	//	ngxData.IRS_annunALIGN[1] = shareMemSDK->IRS_annunALIGN[1];
	//	js["IRS_annunALIGN_2"] = shareMemSDK->IRS_annunALIGN[1] != 0;
	//}
	//if (ngxData.IRS_annunON_DC[0] != shareMemSDK->IRS_annunON_DC[0]) {
	//	ngxData.IRS_annunON_DC[0] = shareMemSDK->IRS_annunON_DC[0];
	//	js["IRS_annunON_DC_1"] = shareMemSDK->IRS_annunON_DC[0] != 0;
	//}
	//if (ngxData.IRS_annunON_DC[1] != shareMemSDK->IRS_annunON_DC[1]) {
	//	ngxData.IRS_annunON_DC[1] = shareMemSDK->IRS_annunON_DC[1];
	//	js["IRS_annunON_DC_2"] = shareMemSDK->IRS_annunON_DC[1] != 0;
	//}
	//if (ngxData.IRS_annunFAULT[0] != shareMemSDK->IRS_annunFAULT[0]) {
	//	ngxData.IRS_annunFAULT[0] = shareMemSDK->IRS_annunFAULT[0];
	//	js["IRS_annunFAULT_1"] = shareMemSDK->IRS_annunFAULT[0] != 0;
	//}
	//if (ngxData.IRS_annunFAULT[1] != shareMemSDK->IRS_annunFAULT[1]) {
	//	ngxData.IRS_annunFAULT[1] = shareMemSDK->IRS_annunFAULT[1];
	//	js["IRS_annunFAULT_2"] = shareMemSDK->IRS_annunFAULT[1] != 0;
	//}
	//if (ngxData.IRS_annunDC_FAIL[0] != shareMemSDK->IRS_annunDC_FAIL[0]) {
	//	ngxData.IRS_annunDC_FAIL[0] = shareMemSDK->IRS_annunDC_FAIL[0];
	//	js["IRS_annunDC_FAIL_1"] = shareMemSDK->IRS_annunDC_FAIL[0] != 0;
	//}
	//if (ngxData.IRS_annunDC_FAIL[1] != shareMemSDK->IRS_annunDC_FAIL[1]) {
	//	ngxData.IRS_annunDC_FAIL[1] = shareMemSDK->IRS_annunDC_FAIL[1];
	//	js["IRS_annunDC_FAIL_2"] = shareMemSDK->IRS_annunDC_FAIL[1] != 0;
	//}
	if (ngxData.REVERSER_1_Light_Status != shareMemSDK->REVERSER_1_Light_Status) {
		ngxData.REVERSER_1_Light_Status = shareMemSDK->REVERSER_1_Light_Status;
		js["ENG_annunREVERSER_1"] = shareMemSDK->REVERSER_1_Light_Status != 0;
	}
	if (ngxData.REVERSER_2_Light_Status != shareMemSDK->REVERSER_2_Light_Status) {
		ngxData.REVERSER_2_Light_Status = shareMemSDK->REVERSER_2_Light_Status;
		js["ENG_annunREVERSER_2"] = shareMemSDK->REVERSER_2_Light_Status != 0;
	}
	if (ngxData.ENGINE_CONTROL_1_Light_Status != shareMemSDK->ENGINE_CONTROL_1_Light_Status) {
		ngxData.ENGINE_CONTROL_1_Light_Status = shareMemSDK->ENGINE_CONTROL_1_Light_Status;
		js["ENG_annunENGINE_CONTROL_1"] = shareMemSDK->ENGINE_CONTROL_1_Light_Status != 0;
	}
	if (ngxData.ENGINE_CONTROL_2_Light_Status != shareMemSDK->ENGINE_CONTROL_2_Light_Status) {
		ngxData.ENGINE_CONTROL_2_Light_Status = shareMemSDK->ENGINE_CONTROL_2_Light_Status;
		js["ENG_annunENGINE_CONTROL_2"] = shareMemSDK->ENGINE_CONTROL_2_Light_Status != 0;
	}
	//if (ngxData.ENG_annunALTN[0] != shareMemSDK->ENG_annunALTN[0]) {
	//	ngxData.ENG_annunALTN[0] = shareMemSDK->ENG_annunALTN[0];
	//	js["ENG_annunALTN_1"] = shareMemSDK->ENG_annunALTN[0] != 0;
	//}
	//if (ngxData.ENG_annunALTN[1] != shareMemSDK->ENG_annunALTN[1]) {
	//	ngxData.ENG_annunALTN[1] = shareMemSDK->ENG_annunALTN[1];
	//	js["ENG_annunALTN_2"] = shareMemSDK->ENG_annunALTN[1] != 0;
	//}
	if (ngxData.LOW_PRESSURE_A_Light_Status != shareMemSDK->LOW_PRESSURE_A_Light_Status) {
		ngxData.LOW_PRESSURE_A_Light_Status = shareMemSDK->LOW_PRESSURE_A_Light_Status;
		js["FCTL_annunFC_LOW_PRESSURE_1"] = shareMemSDK->LOW_PRESSURE_A_Light_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_B_Light_Status != shareMemSDK->LOW_PRESSURE_B_Light_Status) {
		ngxData.LOW_PRESSURE_B_Light_Status = shareMemSDK->LOW_PRESSURE_B_Light_Status;
		js["FCTL_annunFC_LOW_PRESSURE_2"] = shareMemSDK->LOW_PRESSURE_B_Light_Status != 0;
	}
	if (ngxData.ENG_VALVE_CLOSED_Light_1_Status != shareMemSDK->ENG_VALVE_CLOSED_Light_1_Status) {
		ngxData.ENG_VALVE_CLOSED_Light_1_Status = shareMemSDK->ENG_VALVE_CLOSED_Light_1_Status;
		js["FUEL_annunENG_VALVE_CLOSED_1"] = shareMemSDK->ENG_VALVE_CLOSED_Light_1_Status != 0 != 0;
	}
	if (ngxData.ENG_VALVE_CLOSED_Light_2_Status != shareMemSDK->ENG_VALVE_CLOSED_Light_2_Status) {
		ngxData.ENG_VALVE_CLOSED_Light_2_Status = shareMemSDK->ENG_VALVE_CLOSED_Light_2_Status;
		js["FUEL_annunENG_VALVE_CLOSED_2"] = shareMemSDK->ENG_VALVE_CLOSED_Light_2_Status != 0 != 0;

	}
	if (ngxData.SPAR_VALVE_CLOSED_Light_1_Status != shareMemSDK->SPAR_VALVE_CLOSED_Light_1_Status) {
		ngxData.SPAR_VALVE_CLOSED_Light_1_Status = shareMemSDK->SPAR_VALVE_CLOSED_Light_1_Status;
		js["FUEL_annunSPAR_VALVE_CLOSED_1"] = shareMemSDK->SPAR_VALVE_CLOSED_Light_1_Status != 0 != 0;

	}
	if (ngxData.SPAR_VALVE_CLOSED_Light_2_Status != shareMemSDK->SPAR_VALVE_CLOSED_Light_2_Status) {
		ngxData.SPAR_VALVE_CLOSED_Light_2_Status = shareMemSDK->SPAR_VALVE_CLOSED_Light_2_Status;
		js["FUEL_annunSPAR_VALVE_CLOSED_2"] = shareMemSDK->SPAR_VALVE_CLOSED_Light_2_Status != 0 != 0;
	}
	if (ngxData.FILTER_BYPASS_Light_1_Status != shareMemSDK->FILTER_BYPASS_Light_1_Status) {
		ngxData.FILTER_BYPASS_Light_1_Status = shareMemSDK->FILTER_BYPASS_Light_1_Status;
		js["FUEL_annunFILTER_BYPASS_1"] = shareMemSDK->FILTER_BYPASS_Light_1_Status != 0;
	}
	if (ngxData.FILTER_BYPASS_Light_2_Status != shareMemSDK->FILTER_BYPASS_Light_2_Status) {
		ngxData.FILTER_BYPASS_Light_2_Status = shareMemSDK->FILTER_BYPASS_Light_2_Status;
		js["FUEL_annunFILTER_BYPASS_2"] = shareMemSDK->FILTER_BYPASS_Light_2_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_Light_L_FWD_Status != shareMemSDK->LOW_PRESSURE_Light_L_FWD_Status) {
		ngxData.LOW_PRESSURE_Light_L_FWD_Status = shareMemSDK->LOW_PRESSURE_Light_L_FWD_Status;
		js["FUEL_annunLOWPRESS_Fwd_1"] = shareMemSDK->LOW_PRESSURE_Light_L_FWD_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_Light_R_FWD_Status != shareMemSDK->LOW_PRESSURE_Light_R_FWD_Status) {
		ngxData.LOW_PRESSURE_Light_R_FWD_Status = shareMemSDK->LOW_PRESSURE_Light_R_FWD_Status;
		js["FUEL_annunLOWPRESS_Fwd_2"] = shareMemSDK->LOW_PRESSURE_Light_R_FWD_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_Light_L_AFT_Status != shareMemSDK->LOW_PRESSURE_Light_L_AFT_Status) {
		ngxData.LOW_PRESSURE_Light_L_AFT_Status = shareMemSDK->LOW_PRESSURE_Light_L_AFT_Status;
		js["FUEL_annunLOWPRESS_Aft_1"] = shareMemSDK->LOW_PRESSURE_Light_L_AFT_Status != 0;
	}
	if (ngxData.LOW_PRESSURE_Light_R_AFT_Status != shareMemSDK->LOW_PRESSURE_Light_R_AFT_Status) {
		ngxData.LOW_PRESSURE_Light_R_AFT_Status = shareMemSDK->LOW_PRESSURE_Light_R_AFT_Status;
		js["FUEL_annunLOWPRESS_Aft_2"] = shareMemSDK->LOW_PRESSURE_Light_R_AFT_Status != 0;
	}
	if (ngxData.Fuel_CENTER_L_Switch_Status != shareMemSDK->Fuel_CENTER_L_Switch_Status) {
		ngxData.Fuel_CENTER_L_Switch_Status = shareMemSDK->Fuel_CENTER_L_Switch_Status;
		js["FUEL_annunLOWPRESS_Ctr_1"] = shareMemSDK->Fuel_CENTER_L_Switch_Status != 0;
	}
	if (ngxData.Fuel_CENTER_R_Switch_Status != shareMemSDK->Fuel_CENTER_R_Switch_Status) {
		ngxData.Fuel_CENTER_R_Switch_Status = shareMemSDK->Fuel_CENTER_R_Switch_Status;
		js["FUEL_annunLOWPRESS_Ctr_2"] = shareMemSDK->Fuel_CENTER_R_Switch_Status != 0;
	}
	if (ngxData.ENG_1_DRIVE_Light_Status != shareMemSDK->ENG_1_DRIVE_Light_Status) {
		ngxData.ENG_1_DRIVE_Light_Status = shareMemSDK->ENG_1_DRIVE_Light_Status;
		js["ELEC_annunDRIVE_1"] = shareMemSDK->ENG_1_DRIVE_Light_Status != 0;
	}
	if (ngxData.ENG_2_DRIVE_Light_Status != shareMemSDK->ENG_2_DRIVE_Light_Status) {
		ngxData.ENG_2_DRIVE_Light_Status = shareMemSDK->ENG_2_DRIVE_Light_Status;
		js["ELEC_annunDRIVE_2"] = shareMemSDK->ENG_2_DRIVE_Light_Status != 0;
	}
	if (ngxData.ENG_1_TRANSFER_BUS_OFF_Light_Status != shareMemSDK->ENG_1_TRANSFER_BUS_OFF_Light_Status) {
		ngxData.ENG_1_TRANSFER_BUS_OFF_Light_Status = shareMemSDK->ENG_1_TRANSFER_BUS_OFF_Light_Status;
		js["ELEC_annunTRANSFER_BUS_OFF_1"] = shareMemSDK->ENG_1_TRANSFER_BUS_OFF_Light_Status != 0;
	}
	if (ngxData.ENG_2_TRANSFER_BUS_OFF_Light_Status != shareMemSDK->ENG_2_TRANSFER_BUS_OFF_Light_Status) {
		ngxData.ENG_2_TRANSFER_BUS_OFF_Light_Status = shareMemSDK->ENG_2_TRANSFER_BUS_OFF_Light_Status;
		js["ELEC_annunTRANSFER_BUS_OFF_2"] = shareMemSDK->ENG_2_TRANSFER_BUS_OFF_Light_Status != 0;
	}
	if (ngxData.ENG_1_SOURCE_OFF_Light_Status != shareMemSDK->ENG_1_SOURCE_OFF_Light_Status) {
		ngxData.ENG_1_SOURCE_OFF_Light_Status = shareMemSDK->ENG_1_SOURCE_OFF_Light_Status;
		js["ELEC_annunSOURCE_OFF_1"] = shareMemSDK->ENG_1_SOURCE_OFF_Light_Status != 0;
	}
	if (ngxData.ENG_2_SOURCE_OFF_Light_Status != shareMemSDK->ENG_2_SOURCE_OFF_Light_Status) {
		ngxData.ENG_2_SOURCE_OFF_Light_Status = shareMemSDK->ENG_2_SOURCE_OFF_Light_Status;
		js["ELEC_annunSOURCE_OFF_2"] = shareMemSDK->ENG_2_SOURCE_OFF_Light_Status != 0;
	}
	if (ngxData.ENG_1_GEN_OFF_BUS_Light_Status != shareMemSDK->ENG_1_GEN_OFF_BUS_Light_Status) {
		ngxData.ENG_1_GEN_OFF_BUS_Light_Status = shareMemSDK->ENG_1_GEN_OFF_BUS_Light_Status;
		js["ELEC_annunGEN_BUS_OFF_1"] = shareMemSDK->ENG_1_GEN_OFF_BUS_Light_Status != 0;
	}
	if (ngxData.ENG_2_GEN_OFF_BUS_Light_Status != shareMemSDK->ENG_2_GEN_OFF_BUS_Light_Status) {
		ngxData.ENG_2_GEN_OFF_BUS_Light_Status = shareMemSDK->ENG_2_GEN_OFF_BUS_Light_Status;
		js["ELEC_annunGEN_BUS_OFF_2"] = shareMemSDK->ENG_2_GEN_OFF_BUS_Light_Status != 0;
	}
	if (ngxData.Window_OVERHEAT_Lights_1_Status != shareMemSDK->Window_OVERHEAT_Lights_1_Status) {
		ngxData.Window_OVERHEAT_Lights_1_Status = shareMemSDK->Window_OVERHEAT_Lights_1_Status;
		js["ICE_annunOVERHEAT_1"] = shareMemSDK->Window_OVERHEAT_Lights_1_Status != 0;
	}
	if (ngxData.Window_OVERHEAT_Lights_2_Status != shareMemSDK->Window_OVERHEAT_Lights_2_Status) {
		ngxData.Window_OVERHEAT_Lights_2_Status = shareMemSDK->Window_OVERHEAT_Lights_2_Status;
		js["ICE_annunOVERHEAT_2"] = shareMemSDK->Window_OVERHEAT_Lights_2_Status != 0;
	}
	if (ngxData.Window_OVERHEAT_Lights_3_Status != shareMemSDK->Window_OVERHEAT_Lights_3_Status) {
		ngxData.Window_OVERHEAT_Lights_3_Status = shareMemSDK->Window_OVERHEAT_Lights_3_Status;
		js["ICE_annunOVERHEAT_3"] = shareMemSDK->Window_OVERHEAT_Lights_3_Status != 0;
	}
	if (ngxData.Window_OVERHEAT_Lights_4_Status != shareMemSDK->Window_OVERHEAT_Lights_4_Status) {
		ngxData.Window_OVERHEAT_Lights_4_Status = shareMemSDK->Window_OVERHEAT_Lights_4_Status;
		js["ICE_annunOVERHEAT_4"] = shareMemSDK->Window_OVERHEAT_Lights_4_Status != 0;
	}
	if (ngxData.Window_Heat_ON_Lights_1_Status != shareMemSDK->Window_Heat_ON_Lights_1_Status) {
		ngxData.Window_Heat_ON_Lights_1_Status = shareMemSDK->Window_Heat_ON_Lights_1_Status;
		js["ICE_annunON_1"] = shareMemSDK->Window_Heat_ON_Lights_1_Status != 0;
	}
	if (ngxData.Window_Heat_ON_Lights_2_Status != shareMemSDK->Window_Heat_ON_Lights_2_Status) {
		ngxData.Window_Heat_ON_Lights_2_Status = shareMemSDK->Window_Heat_ON_Lights_2_Status;
		js["ICE_annunON_2"] = shareMemSDK->Window_Heat_ON_Lights_2_Status != 0;
	}
	if (ngxData.Window_Heat_ON_Lights_3_Status != shareMemSDK->Window_Heat_ON_Lights_3_Status) {
		ngxData.Window_Heat_ON_Lights_3_Status = shareMemSDK->Window_Heat_ON_Lights_3_Status;
		js["ICE_annunON_3"] = shareMemSDK->Window_Heat_ON_Lights_3_Status != 0;
	}
	if (ngxData.Window_Heat_ON_Lights_4_Status != shareMemSDK->Window_Heat_ON_Lights_4_Status) {
		ngxData.Window_Heat_ON_Lights_4_Status = shareMemSDK->Window_Heat_ON_Lights_4_Status;
		js["ICE_annunON_4"] = shareMemSDK->Window_Heat_ON_Lights_4_Status != 0;
	}
	if (ngxData.L_VALVE_OPEN_Lights_Status != shareMemSDK->L_VALVE_OPEN_Lights_Status) {
		ngxData.L_VALVE_OPEN_Lights_Status = shareMemSDK->L_VALVE_OPEN_Lights_Status;
		js["ICE_annunVALVE_OPEN_1"] = shareMemSDK->L_VALVE_OPEN_Lights_Status != 0;
	}
	if (ngxData.R_VALVE_OPEN_Lights_Status != shareMemSDK->R_VALVE_OPEN_Lights_Status) {
		ngxData.R_VALVE_OPEN_Lights_Status = shareMemSDK->R_VALVE_OPEN_Lights_Status;
		js["ICE_annunVALVE_OPEN_2"] = shareMemSDK->R_VALVE_OPEN_Lights_Status != 0;
	}
	if (ngxData.COWL_ANTI_ICE_Lights_1_Status != shareMemSDK->COWL_ANTI_ICE_Lights_1_Status) {
		ngxData.COWL_ANTI_ICE_Lights_1_Status = shareMemSDK->COWL_ANTI_ICE_Lights_1_Status;
		js["ICE_annunCOWL_ANTI_ICE_1"] = shareMemSDK->COWL_ANTI_ICE_Lights_1_Status != 0;
	}
	if (ngxData.COWL_ANTI_ICE_Lights_2_Status != shareMemSDK->COWL_ANTI_ICE_Lights_2_Status) {
		ngxData.COWL_ANTI_ICE_Lights_2_Status = shareMemSDK->COWL_ANTI_ICE_Lights_2_Status;
		js["ICE_annunCOWL_ANTI_ICE_2"] = shareMemSDK->COWL_ANTI_ICE_Lights_2_Status != 0;
	}
	if (ngxData.COWL_VALVE_OPEN_Lights_1_Status != shareMemSDK->COWL_VALVE_OPEN_Lights_1_Status) {
		ngxData.COWL_VALVE_OPEN_Lights_1_Status = shareMemSDK->COWL_VALVE_OPEN_Lights_1_Status;
		js["ICE_annunCOWL_VALVE_OPEN_1"] = shareMemSDK->COWL_VALVE_OPEN_Lights_1_Status != 0;
	}
	if (ngxData.COWL_VALVE_OPEN_Lights_2_Status != shareMemSDK->COWL_VALVE_OPEN_Lights_2_Status) {
		ngxData.COWL_VALVE_OPEN_Lights_2_Status = shareMemSDK->COWL_VALVE_OPEN_Lights_2_Status;
		js["ICE_annunCOWL_VALVE_OPEN_2"] = shareMemSDK->COWL_VALVE_OPEN_Lights_2_Status != 0;
	}
	if (ngxData.ENG_1_HYD_LOW_PRESSURE_Light_Status!= shareMemSDK->ENG_1_HYD_LOW_PRESSURE_Light_Status) {
		ngxData.ENG_1_HYD_LOW_PRESSURE_Light_Status= shareMemSDK->ENG_1_HYD_LOW_PRESSURE_Light_Status;
		js["HYD_annunLOW_PRESS_eng_1"] = shareMemSDK->ENG_1_HYD_LOW_PRESSURE_Light_Status!= 0;
	}
	if (ngxData.ENG_2_HYD_LOW_PRESSURE_Light_Status!= shareMemSDK->ENG_2_HYD_LOW_PRESSURE_Light_Status) {
		ngxData.ENG_2_HYD_LOW_PRESSURE_Light_Status= shareMemSDK->ENG_2_HYD_LOW_PRESSURE_Light_Status;
		js["HYD_annunLOW_PRESS_eng_2"] = shareMemSDK->ENG_2_HYD_LOW_PRESSURE_Light_Status!= 0;
	}
	if (ngxData.ELEC_1_HYD_LOW_PRESSURE_Light_Status != shareMemSDK->ELEC_1_HYD_LOW_PRESSURE_Light_Status) {
		ngxData.ELEC_1_HYD_LOW_PRESSURE_Light_Status = shareMemSDK->ELEC_1_HYD_LOW_PRESSURE_Light_Status;
		js["HYD_annunLOW_PRESS_elec_1"] = shareMemSDK->ELEC_1_HYD_LOW_PRESSURE_Light_Status != 0;
	}
	if (ngxData.ELEC_2_HYD_LOW_PRESSURE_Light_Status != shareMemSDK->ELEC_2_HYD_LOW_PRESSURE_Light_Status) {
		ngxData.ELEC_2_HYD_LOW_PRESSURE_Light_Status = shareMemSDK->ELEC_2_HYD_LOW_PRESSURE_Light_Status;
		js["HYD_annunLOW_PRESS_elec_2"] = shareMemSDK->ELEC_2_HYD_LOW_PRESSURE_Light_Status != 0;
	}
	if (ngxData.ELEC_1_HYD_OVERHEAT_Light_Status != shareMemSDK->ELEC_1_HYD_OVERHEAT_Light_Status) {
		ngxData.ELEC_1_HYD_OVERHEAT_Light_Status = shareMemSDK->ELEC_1_HYD_OVERHEAT_Light_Status;
		js["HYD_annunOVERHEAT_elec_1"] = shareMemSDK->ELEC_1_HYD_OVERHEAT_Light_Status != 0;
	}
	if (ngxData.ELEC_2_HYD_OVERHEAT_Light_Status != shareMemSDK->ELEC_2_HYD_OVERHEAT_Light_Status) {
		ngxData.ELEC_2_HYD_OVERHEAT_Light_Status = shareMemSDK->ELEC_2_HYD_OVERHEAT_Light_Status;
		js["HYD_annunOVERHEAT_elec_2"] = shareMemSDK->ELEC_2_HYD_OVERHEAT_Light_Status != 0;
	}
	if (ngxData.ZONE_TEMP_Light_1_Status != shareMemSDK->ZONE_TEMP_Light_1_Status) {
		ngxData.ZONE_TEMP_Light_1_Status = shareMemSDK->ZONE_TEMP_Light_1_Status;
		js["AIR_annunZoneTemp_1"] = shareMemSDK->ZONE_TEMP_Light_1_Status != 0;
	}
	if (ngxData.ZONE_TEMP_Light_2_Status != shareMemSDK->ZONE_TEMP_Light_2_Status) {
		ngxData.ZONE_TEMP_Light_2_Status = shareMemSDK->ZONE_TEMP_Light_2_Status;
		js["AIR_annunZoneTemp_2"] = shareMemSDK->ZONE_TEMP_Light_2_Status != 0;
	}
	if (ngxData.ZONE_TEMP_Light_3_Status != shareMemSDK->ZONE_TEMP_Light_3_Status) {
		ngxData.ZONE_TEMP_Light_3_Status = shareMemSDK->ZONE_TEMP_Light_3_Status;
		js["AIR_annunZoneTemp_3"] = shareMemSDK->ZONE_TEMP_Light_3_Status != 0;
	}
	if (ngxData.PACK_TRIP_OFF_1_Light_Status != shareMemSDK->PACK_TRIP_OFF_1_Light_Status) {
		ngxData.PACK_TRIP_OFF_1_Light_Status = shareMemSDK->PACK_TRIP_OFF_1_Light_Status;
		js["AIR_annunPackTripOff_1"] = shareMemSDK->PACK_TRIP_OFF_1_Light_Status != 0;
	}
	if (ngxData.PACK_TRIP_OFF_2_Light_Status != shareMemSDK->PACK_TRIP_OFF_2_Light_Status) {
		ngxData.PACK_TRIP_OFF_2_Light_Status = shareMemSDK->PACK_TRIP_OFF_2_Light_Status;
		js["AIR_annunPackTripOff_2"] = shareMemSDK->PACK_TRIP_OFF_2_Light_Status != 0;
	}
	if (ngxData.WING_BODY_OVERHEAT_1_Light_Status != shareMemSDK->WING_BODY_OVERHEAT_1_Light_Status) {
		ngxData.WING_BODY_OVERHEAT_1_Light_Status = shareMemSDK->WING_BODY_OVERHEAT_1_Light_Status;
		js["AIR_annunWingBodyOverheat_1"] = shareMemSDK->WING_BODY_OVERHEAT_1_Light_Status != 0;
	}
	if (ngxData.WING_BODY_OVERHEAT_2_Light_Status != shareMemSDK->WING_BODY_OVERHEAT_2_Light_Status) {
		ngxData.WING_BODY_OVERHEAT_2_Light_Status = shareMemSDK->WING_BODY_OVERHEAT_2_Light_Status;
		js["AIR_annunWingBodyOverheat_2"] = shareMemSDK->WING_BODY_OVERHEAT_2_Light_Status != 0;
	}
	if (ngxData.BLEED_TRIP_OFF_1_Light_Status != shareMemSDK->BLEED_TRIP_OFF_1_Light_Status) {
		ngxData.BLEED_TRIP_OFF_1_Light_Status = shareMemSDK->BLEED_TRIP_OFF_1_Light_Status;
		js["AIR_annunBleedTripOff_1"] = shareMemSDK->BLEED_TRIP_OFF_1_Light_Status != 0;
	}
	if (ngxData.BLEED_TRIP_OFF_2_Light_Status != shareMemSDK->BLEED_TRIP_OFF_2_Light_Status) {
		ngxData.BLEED_TRIP_OFF_2_Light_Status = shareMemSDK->BLEED_TRIP_OFF_2_Light_Status;
		js["AIR_annunBleedTripOff_2"] = shareMemSDK->BLEED_TRIP_OFF_2_Light_Status != 0;
	}
	//if (ngxData.WARN_annunFIRE_WARN[0] != shareMemSDK->WARN_annunFIRE_WARN[0]) {
	//	ngxData.WARN_annunFIRE_WARN[0] = shareMemSDK->WARN_annunFIRE_WARN[0];
	//	js["WARN_annunFIRE_WARN_1"] = shareMemSDK->WARN_annunFIRE_WARN[0] != 0;
	//}
	//if (ngxData.WARN_annunFIRE_WARN[1] != shareMemSDK->WARN_annunFIRE_WARN[1]) {
	//	ngxData.WARN_annunFIRE_WARN[1] = shareMemSDK->WARN_annunFIRE_WARN[1];
	//	js["WARN_annunFIRE_WARN_2"] = shareMemSDK->WARN_annunFIRE_WARN[1] != 0;
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[0] != shareMemSDK->WARN_annunMASTER_CAUTION[0]) {
	//	ngxData.WARN_annunMASTER_CAUTION[0] = shareMemSDK->WARN_annunMASTER_CAUTION[0];
	//	js["WARN_annunMASTER_CAUTION_1"] = shareMemSDK->WARN_annunMASTER_CAUTION[0] != 0;
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[1] != shareMemSDK->WARN_annunMASTER_CAUTION[1]) {
	//	ngxData.WARN_annunMASTER_CAUTION[1] = shareMemSDK->WARN_annunMASTER_CAUTION[1];
	//	js["WARN_annunMASTER_CAUTION_2"] = shareMemSDK->WARN_annunMASTER_CAUTION[1] != 0;
	//}
	//if (ngxData.MAIN_annunBELOW_GS[0] != shareMemSDK->MAIN_annunBELOW_GS[0]) {
	//	ngxData.MAIN_annunBELOW_GS[0] = shareMemSDK->MAIN_annunBELOW_GS[0];
	//	js["MAIN_annunBELOW_GS_1"] = shareMemSDK->MAIN_annunBELOW_GS[0] != 0;
	//}
	//if (ngxData.MAIN_annunBELOW_GS[1] != shareMemSDK->MAIN_annunBELOW_GS[1]) {
	//	ngxData.MAIN_annunBELOW_GS[1] = shareMemSDK->MAIN_annunBELOW_GS[1];
	//	js["MAIN_annunBELOW_GS_2"] = shareMemSDK->MAIN_annunBELOW_GS[1] != 0;
	//}
	//if (ngxData.MAIN_annunAP[0] != shareMemSDK->MAIN_annunAP[0]) {
	//	ngxData.MAIN_annunAP[0] = shareMemSDK->MAIN_annunAP[0];
	//	js["MAIN_annunAP_1"] = shareMemSDK->MAIN_annunAP[0] != 0;
	//}
	//if (ngxData.MAIN_annunAP[1] != shareMemSDK->MAIN_annunAP[1]) {
	//	ngxData.MAIN_annunAP[1] = shareMemSDK->MAIN_annunAP[1];
	//	js["MAIN_annunAP_2"] = shareMemSDK->MAIN_annunAP[1] != 0;
	//}
	//if (ngxData.MAIN_annunAT[0] != shareMemSDK->MAIN_annunAT[0]) {
	//	ngxData.MAIN_annunAT[0] = shareMemSDK->MAIN_annunAT[0];
	//	js["MAIN_annunAT_1"] = shareMemSDK->MAIN_annunAT[0] != 0;
	//}
	//if (ngxData.MAIN_annunAT[1] != shareMemSDK->MAIN_annunAT[1]) {
	//	ngxData.MAIN_annunAT[1] = shareMemSDK->MAIN_annunAT[1];
	//	js["MAIN_annunAT_2"] = shareMemSDK->MAIN_annunAT[1] != 0;
	//}
	//if (ngxData.MAIN_annunFMC[0] != shareMemSDK->MAIN_annunFMC[0]) {
	//	ngxData.MAIN_annunFMC[0] = shareMemSDK->MAIN_annunFMC[0];
	//	js["MAIN_annunFMC_1"] = shareMemSDK->MAIN_annunFMC[0] != 0;
	//}
	//if (ngxData.MAIN_annunFMC[1] != shareMemSDK->MAIN_annunFMC[1]) {
	//	ngxData.MAIN_annunFMC[1] = shareMemSDK->MAIN_annunFMC[1];
	//	js["MAIN_annunFMC_2"] = shareMemSDK->MAIN_annunFMC[1] != 0;
	//}
	if (ngxData.LEFT_GEAR_RedLight_Status != shareMemSDK->LEFT_GEAR_RedLight_Status) {
		ngxData.LEFT_GEAR_RedLight_Status = shareMemSDK->LEFT_GEAR_RedLight_Status;
		js["MAIN_annunGEAR_transit_1"] = shareMemSDK->LEFT_GEAR_RedLight_Status != 0;
	}
	if (ngxData.NOSE_GEAR_RedLight_Status != shareMemSDK->NOSE_GEAR_RedLight_Status) {
		ngxData.NOSE_GEAR_RedLight_Status = shareMemSDK->NOSE_GEAR_RedLight_Status;
		js["MAIN_annunGEAR_transit_2"] = shareMemSDK->NOSE_GEAR_RedLight_Status != 0;
	}
	if (ngxData.RIGHT_GEAR_RedLight_Status != shareMemSDK->RIGHT_GEAR_RedLight_Status) {
		ngxData.RIGHT_GEAR_RedLight_Status = shareMemSDK->RIGHT_GEAR_RedLight_Status;
		js["MAIN_annunGEAR_transit_3"] = shareMemSDK->RIGHT_GEAR_RedLight_Status != 0;
	}


	//if (ngxData.FIRE_annunENG_OVERHEAT[0] != shareMemSDK->FIRE_annunENG_OVERHEAT[0]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[0] = shareMemSDK->FIRE_annunENG_OVERHEAT[0];
	//	js["FIRE_annunENG_OVERHEAT_1"] = shareMemSDK->FIRE_annunENG_OVERHEAT[0] != 0;
	//}
	//if (ngxData.FIRE_annunENG_OVERHEAT[1] != shareMemSDK->FIRE_annunENG_OVERHEAT[1]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[1] = shareMemSDK->FIRE_annunENG_OVERHEAT[1];
	//	js["FIRE_annunENG_OVERHEAT_2"] = shareMemSDK->FIRE_annunENG_OVERHEAT[1] != 0;
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[0] != shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[0]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[0] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[0];
	//	js["FIRE_annunBOTTLE_DISCHARGE_1"] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[0] != 0;
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[1] != shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[1]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[1] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[1];
	//	js["FIRE_annunBOTTLE_DISCHARGE_2"] = shareMemSDK->FIRE_annunBOTTLE_DISCHARGE[1] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[0] != shareMemSDK->FIRE_annunExtinguisherTest[0]) {
	//	ngxData.FIRE_annunExtinguisherTest[0] = shareMemSDK->FIRE_annunExtinguisherTest[0];
	//	js["FIRE_annunExtinguisherTest_1"] = shareMemSDK->FIRE_annunExtinguisherTest[0] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[1] != shareMemSDK->FIRE_annunExtinguisherTest[1]) {
	//	ngxData.FIRE_annunExtinguisherTest[1] = shareMemSDK->FIRE_annunExtinguisherTest[1];
	//	js["FIRE_annunExtinguisherTest_2"] = shareMemSDK->FIRE_annunExtinguisherTest[1] != 0;
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[2] != shareMemSDK->FIRE_annunExtinguisherTest[2]) {
	//	ngxData.FIRE_annunExtinguisherTest[2] = shareMemSDK->FIRE_annunExtinguisherTest[2];
	//	js["FIRE_annunExtinguisherTest_3"] = shareMemSDK->FIRE_annunExtinguisherTest[2] != 0;
	//}
	//if (ngxData.CARGO_annunExtTest[0] != shareMemSDK->CARGO_annunExtTest[0]) {
	//	ngxData.CARGO_annunExtTest[0] = shareMemSDK->CARGO_annunExtTest[0];
	//	js["CARGO_annunExtTest_1"] = shareMemSDK->CARGO_annunExtTest[0] != 0;
	//}
	//if (ngxData.CARGO_annunExtTest[1] != shareMemSDK->CARGO_annunExtTest[1]) {
	//	ngxData.CARGO_annunExtTest[1] = shareMemSDK->CARGO_annunExtTest[1];
	//	js["CARGO_annunExtTest_2"] = shareMemSDK->CARGO_annunExtTest[1] != 0;
	//}
	//if (ngxData.MAIN_annunAP_Amber[0] != shareMemSDK->MAIN_annunAP_Amber[0]) {
	//	ngxData.MAIN_annunAP_Amber[0] = shareMemSDK->MAIN_annunAP_Amber[0];
	//	js["MAIN_annunAP_Amber_1"] = shareMemSDK->MAIN_annunAP_Amber[0] != 0;
	//}
	//if (ngxData.MAIN_annunAP_Amber[1] != shareMemSDK->MAIN_annunAP_Amber[1]) {
	//	ngxData.MAIN_annunAP_Amber[1] = shareMemSDK->MAIN_annunAP_Amber[1];
	//	js["MAIN_annunAP_Amber_2"] = shareMemSDK->MAIN_annunAP_Amber[1] != 0;
	//}
	//if (ngxData.MAIN_annunAT_Amber[0] != shareMemSDK->MAIN_annunAT_Amber[0]) {
	//	ngxData.MAIN_annunAT_Amber[0] = shareMemSDK->MAIN_annunAT_Amber[0];
	//	js["MAIN_annunAT_Amber_1"] = shareMemSDK->MAIN_annunAT_Amber[0] != 0;
	//}
	//if (ngxData.MAIN_annunAT_Amber[1] != shareMemSDK->MAIN_annunAT_Amber[1]) {
	//	ngxData.MAIN_annunAT_Amber[1] = shareMemSDK->MAIN_annunAT_Amber[1];
	//	js["MAIN_annunAT_Amber_2"] = shareMemSDK->MAIN_annunAT_Amber[1] != 0;
	//}
	
	return js.dump();

}