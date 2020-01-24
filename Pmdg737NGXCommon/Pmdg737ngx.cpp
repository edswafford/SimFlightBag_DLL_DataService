#include "Pmdg737ngx.h"
#include "logger/logger.h"

extern logger LOG;

using json = nlohmann::json;
bool B737ngx::initialization_requested = false;
bool B737ngx::initialized  = false;
std::string B737ngx::processPmdg737(const AIRCRAFT_DATA_TYPE& data)
{
	std::string json_string;
	if (!initialized || B737ngx::initialization_requested) {

		try {
			memcpy(&ngxData, &data, sizeof(AIRCRAFT_DATA_TYPE));
			json_string = initialize(data);
			if (json_string != "null") {
				json js = {
					{"messageType", "737ngx"},
					{"Initialize", json::parse(json_string)}
				};

				B737ngx::initialization_requested = false;
				initialized = true;

				return js.dump();
			}
		}
		catch(std::exception e)
		{
			LOG() << "Failed to initial 737 NGX json data " << e.what();
		}


	}
	else if (initialized)
	{
		try {
			json_string = buildJsonPmdg737(data);
			if (json_string != "null") {
				json js = {
					{"messageType", "737ngx"},
					{"AircraftChanges", json::parse(json_string)}
				};
				return js.dump();
			}
		}
		catch(std::exception e)
		{
			LOG() << "Failed to format 737 NGX json data " << e.what();
		}
	}

	return "";
}

void B737ngx::requestInitialData()
{
	initialization_requested = true;
}

void B737ngx::clearInitialDataRequest()
{
	initialization_requested = false;
}

bool B737ngx::initialDataRequested()
{
	return  initialization_requested;
}
bool B737ngx::isInitialized()
{
	return initialized;
}

std::string B737ngx::initialize(const AIRCRAFT_DATA_TYPE & data)
{
	bool valid = false;
	if(data.AircraftModel >= 1 && data.AircraftModel <= 7)
	{
		valid = true;
	}
	json js;
	js["DataIsValid"] = valid;
	js["OXY_Needle"] = data.OXY_Needle;

	js["IRS_DisplaySelector"] = data.IRS_DisplaySelector;
	js["IRS_SysDisplay_R"] = data.IRS_SysDisplay_R;
	js["COMM_ServiceInterphoneSw"] = data.COMM_ServiceInterphoneSw;
	js["OXY_SwNormal"] = data.OXY_SwNormal;
#ifdef PMDG_737_NG3 
	js["FLTREC_SwNormal"] = !data.FLTREC_SwNormal;
#else
	js["FLTREC_SwNormal"] = data.FLTREC_SwNormal;
#endif
	js["FCTL_YawDamper_Sw"] = data.FCTL_YawDamper_Sw;
	js["FCTL_AltnFlaps_Sw_ARM"] = data.FCTL_AltnFlaps_Sw_ARM;
	js["FCTL_AltnFlaps_Control_Sw"] = data.FCTL_AltnFlaps_Control_Sw;
	js["NAVDIS_VHFNavSelector"] = data.NAVDIS_VHFNavSelector;
	js["NAVDIS_IRSSelector"] = data.NAVDIS_IRSSelector;
	js["NAVDIS_FMCSelector"] = data.NAVDIS_FMCSelector;
	js["NAVDIS_SourceSelector"] = data.NAVDIS_SourceSelector;
	js["NAVDIS_ControlPaneSelector"] = data.NAVDIS_ControlPaneSelector;
	js["FUEL_CrossFeedSw"] = data.FUEL_CrossFeedSw;
	js["ELEC_DCMeterSelector"] = data.ELEC_DCMeterSelector;
	js["ELEC_ACMeterSelector"] = data.ELEC_ACMeterSelector;
	js["ELEC_BatSelector"] = data.ELEC_BatSelector;
	js["ELEC_CabUtilSw"] = data.ELEC_CabUtilSw;
	js["ELEC_IFEPassSeatSw"] = data.ELEC_IFEPassSeatSw;
	js["ELEC_StandbyPowerSelector"] = data.ELEC_StandbyPowerSelector;
	js["ELEC_GrdPwrSw"] = data.ELEC_GrdPwrSw;
	js["ELEC_BusTransSw_AUTO"] = data.ELEC_BusTransSw_AUTO;
	js["OH_WiperLSelector"] = data.OH_WiperLSelector;
	js["OH_WiperRSelector"] = data.OH_WiperRSelector;
	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	js["LTS_CircuitBreakerKnob"] = data.LTS_CircuitBreakerKnob;
	js["LTS_OvereadPanelKnob"] = data.LTS_OvereadPanelKnob;
	js["AIR_EquipCoolingSupplyNORM"] = data.AIR_EquipCoolingSupplyNORM;
	js["AIR_EquipCoolingExhaustNORM"] = data.AIR_EquipCoolingExhaustNORM;
	js["LTS_EmerExitSelector"] = data.LTS_EmerExitSelector;
	js["COMM_NoSmokingSelector"] = data.COMM_NoSmokingSelector;
	js["COMM_FastenBeltsSelector"] = data.COMM_FastenBeltsSelector;
	js["ICE_WingAntiIceSw"] = data.ICE_WingAntiIceSw;
	js["AIR_TempSourceSelector"] = data.AIR_TempSourceSelector;
	js["AIR_TrimAirSwitch"] = data.AIR_TrimAirSwitch;
	js["AIR_APUBleedAirSwitch"] = data.AIR_APUBleedAirSwitch;
	js["AIR_IsolationValveSwitch"] = data.AIR_IsolationValveSwitch;
	js["AIR_OutflowValveSwitch"] = data.AIR_OutflowValveSwitch;
	js["AIR_PressurizationModeSelector"] = data.AIR_PressurizationModeSelector;
	js["LTS_TaxiSw"] = data.LTS_TaxiSw;
	js["APU_Selector"] = data.APU_Selector;
	js["ENG_IgnitionSelector"] = data.ENG_IgnitionSelector;
	js["LTS_LogoSw"] = data.LTS_LogoSw;
	js["LTS_PositionSw"] = data.LTS_PositionSw;
	js["LTS_AntiCollisionSw"] = data.LTS_AntiCollisionSw;
	js["LTS_WingSw"] = data.LTS_WingSw;
	js["LTS_WheelWellSw"] = data.LTS_WheelWellSw;
	js["MAIN_NoseWheelSteeringSwNORM"] = data.MAIN_NoseWheelSteeringSwNORM;
	js["MAIN_LightsSelector"] = data.MAIN_LightsSelector;
	js["MAIN_RMISelector1_VOR"] = data.MAIN_RMISelector1_VOR;
	js["MAIN_RMISelector2_VOR"] = data.MAIN_RMISelector2_VOR;
	js["MAIN_N1SetSelector"] = data.MAIN_N1SetSelector;
	js["MAIN_SpdRefSelector"] = data.MAIN_SpdRefSelector;
	js["MAIN_FuelFlowSelector"] = data.MAIN_FuelFlowSelector;
	js["MAIN_AutobrakeSelector"] = data.MAIN_AutobrakeSelector;
	js["MAIN_GearLever"] = data.MAIN_GearLever;
	js["GPWS_FlapInhibitSw_NORM"] = data.GPWS_FlapInhibitSw_NORM;
	js["GPWS_GearInhibitSw_NORM"] = data.GPWS_GearInhibitSw_NORM;
	js["GPWS_TerrInhibitSw_NORM"] = data.GPWS_TerrInhibitSw_NORM;
	js["TRIM_StabTrimMainElecSw_NORMAL"] = data.TRIM_StabTrimMainElecSw_NORMAL;
	js["TRIM_StabTrimAutoPilotSw_NORMAL"] = data.TRIM_StabTrimAutoPilotSw_NORMAL;
	js["FIRE_DetTestSw"] = data.FIRE_DetTestSw;
	js["FIRE_ExtinguisherTestSw"] = data.FIRE_ExtinguisherTestSw;
	js["XPDR_XpndrSelector_2"] = data.XPDR_XpndrSelector_2;
	js["XPDR_AltSourceSel_2"] = data.XPDR_AltSourceSel_2;
	js["XPDR_ModeSel"] = data.XPDR_ModeSel;
	js["TRIM_StabTrimSw_NORMAL"] = data.TRIM_StabTrimSw_NORMAL;
	js["PED_FltDkDoorSel"] = data.PED_FltDkDoorSel;
	js["IRS_aligned"] = data.IRS_aligned;
	js["AircraftModel"] = data.AircraftModel;
	js["WeightInKg"] = data.WeightInKg;
	js["GPWS_V1CallEnabled"] = data.GPWS_V1CallEnabled;
	js["GroundConnAvailable"] = data.GroundConnAvailable;
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
	js["FMC_PerfInputComplete"] = data.FMC_PerfInputComplete;
	js["FMC_DistanceToTOD"] = data.FMC_DistanceToTOD;
	js["FMC_DistanceToDest"] = data.FMC_DistanceToDest;
	js["ICE_WindowHeatTestSw"] = data.ICE_WindowHeatTestSw;
	js["IRS_ModeSelector[2]"] = data.IRS_ModeSelector[2];
	js["IRS_ModeSelector_1"] = data.IRS_ModeSelector[0];
	js["IRS_ModeSelector_2"] = data.IRS_ModeSelector[1];
	js["ENG_EECSwitch_1"] = data.ENG_EECSwitch[0];
	js["ENG_EECSwitch_2"] = data.ENG_EECSwitch[1];
	js["FCTL_FltControl_Sw_1"] = data.FCTL_FltControl_Sw[0];
	js["FCTL_FltControl_Sw_2"] = data.FCTL_FltControl_Sw[1];
	js["FCTL_Spoiler_Sw_1"] = data.FCTL_Spoiler_Sw[0];
	js["FCTL_Spoiler_Sw_2"] = data.FCTL_Spoiler_Sw[1];
	js["FUEL_PumpFwdSw_1"] = data.FUEL_PumpFwdSw[0];
	js["FUEL_PumpFwdSw_2"] = data.FUEL_PumpFwdSw[1];
	js["FUEL_PumpAftSw_1"] = data.FUEL_PumpAftSw[0];
	js["FUEL_PumpAftSw_2"] = data.FUEL_PumpAftSw[1];
	js["FUEL_PumpCtrSw_1"] = data.FUEL_PumpCtrSw[0];
	js["FUEL_PumpCtrSw_2"] = data.FUEL_PumpCtrSw[1];
	js["ELEC_IDGDisconnectSw_1"] = data.ELEC_IDGDisconnectSw[0];
	js["ELEC_IDGDisconnectSw_2"] = data.ELEC_IDGDisconnectSw[1];
	js["ELEC_GenSw_1"] = data.ELEC_GenSw[0];
	js["ELEC_GenSw_2"] = data.ELEC_GenSw[1];
	js["ELEC_APUGenSw_1"] = data.ELEC_APUGenSw[0];
	js["ELEC_APUGenSw_2"] = data.ELEC_APUGenSw[1];
	js["ICE_WindowHeatSw_1"] = data.ICE_WindowHeatSw[0];
	js["ICE_WindowHeatSw_2"] = data.ICE_WindowHeatSw[1];
	js["ICE_WindowHeatSw_3"] = data.ICE_WindowHeatSw[2];
	js["ICE_WindowHeatSw_4"] = data.ICE_WindowHeatSw[3];
	js["ICE_ProbeHeatSw_1"] = data.ICE_ProbeHeatSw[0];
	js["ICE_ProbeHeatSw_2"] = data.ICE_ProbeHeatSw[1];
	js["ICE_EngAntiIceSw_1"] = data.ICE_EngAntiIceSw[0];
	js["ICE_EngAntiIceSw_2"] = data.ICE_EngAntiIceSw[1];
	js["HYD_PumpSw_eng_1"] = data.HYD_PumpSw_eng[0];
	js["HYD_PumpSw_eng_2"] = data.HYD_PumpSw_eng[1];
	js["HYD_PumpSw_elec_1"] = data.HYD_PumpSw_elec[0];
	js["HYD_PumpSw_elec_2"] = data.HYD_PumpSw_elec[1];
	js["AIR_RecircFanSwitch_1"] = data.AIR_RecircFanSwitch[0];
	js["AIR_RecircFanSwitch_2"] = data.AIR_RecircFanSwitch[1];
	js["AIR_PackSwitch_1"] = data.AIR_PackSwitch[0];
	js["AIR_PackSwitch_2"] = data.AIR_PackSwitch[1];
	js["AIR_BleedAirSwitch_1"] = data.AIR_BleedAirSwitch[0];
	js["AIR_BleedAirSwitch_2"] = data.AIR_BleedAirSwitch[1];
	js["LTS_LandingLtRetractableSw_1"] = data.LTS_LandingLtRetractableSw[0];
	js["LTS_LandingLtRetractableSw_2"] = data.LTS_LandingLtRetractableSw[1];
	js["LTS_LandingLtFixedSw_1"] = data.LTS_LandingLtFixedSw[0];
	js["LTS_LandingLtFixedSw_2"] = data.LTS_LandingLtFixedSw[1];
	js["LTS_RunwayTurnoffSw_1"] = data.LTS_RunwayTurnoffSw[0];
	js["LTS_RunwayTurnoffSw_2"] = data.LTS_RunwayTurnoffSw[1];
	js["ENG_StartSelector_1"] = data.ENG_StartSelector[0];
	js["ENG_StartSelector_2"] = data.ENG_StartSelector[1];
	js["EFIS_MinsSelBARO_1"] = data.EFIS_MinsSelBARO[0];
	js["EFIS_MinsSelBARO_2"] = data.EFIS_MinsSelBARO[1];
	js["EFIS_BaroSelHPA_1"] = data.EFIS_BaroSelHPA[0];
	js["EFIS_BaroSelHPA_2"] = data.EFIS_BaroSelHPA[1];
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
	js["FIRE_HandleIlluminated_1"] = data.FIRE_HandleIlluminated[0];
	js["FIRE_HandleIlluminated_2"] = data.FIRE_HandleIlluminated[1];
	js["FIRE_HandleIlluminated_3"] = data.FIRE_HandleIlluminated[2];
	js["CARGO_DetSelect_1"] = data.CARGO_DetSelect[0];
	js["CARGO_DetSelect_2"] = data.CARGO_DetSelect[1];
	js["CARGO_ArmedSw_1"] = data.CARGO_ArmedSw[0];
	js["CARGO_ArmedSw_2"] = data.CARGO_ArmedSw[1];
	js["ENG_StartValve_1"] = data.ENG_StartValve[0];
	js["ENG_StartValve_2"] = data.ENG_StartValve[1];
	js["COMM_SelectedMic_1"] = data.COMM_SelectedMic[0];
	js["COMM_SelectedMic_2"] = data.COMM_SelectedMic[1];
	js["COMM_SelectedMic_3"] = data.COMM_SelectedMic[2];
	js["COMM_ReceiverSwitches_1"] = data.COMM_ReceiverSwitches[0];
	js["COMM_ReceiverSwitches_2"] = data.COMM_ReceiverSwitches[1];
	js["COMM_ReceiverSwitches_3"] = data.COMM_ReceiverSwitches[2];
	js["IRS_annunGPS"] = data.IRS_annunGPS;
	js["WARN_annunPSEU"] = data.WARN_annunPSEU;
	js["OXY_annunPASS_OXY_ON"] = data.OXY_annunPASS_OXY_ON;
	js["GEAR_annunOvhdLEFT"] = data.GEAR_annunOvhdLEFT;
	js["GEAR_annunOvhdNOSE"] = data.GEAR_annunOvhdNOSE;
	js["GEAR_annunOvhdRIGHT"] = data.GEAR_annunOvhdRIGHT;
	js["FLTREC_annunOFF"] = data.FLTREC_annunOFF;
	js["FCTL_annunYAW_DAMPER"] = data.FCTL_annunYAW_DAMPER;
	js["FCTL_annunLOW_QUANTITY"] = data.FCTL_annunLOW_QUANTITY;
	js["FCTL_annunLOW_PRESSURE"] = data.FCTL_annunLOW_PRESSURE;
	js["FCTL_annunLOW_STBY_RUD_ON"] = data.FCTL_annunLOW_STBY_RUD_ON;
	js["FCTL_annunFEEL_DIFF_PRESS"] = data.FCTL_annunFEEL_DIFF_PRESS;
	js["FCTL_annunSPEED_TRIM_FAIL"] = data.FCTL_annunSPEED_TRIM_FAIL;
	js["FCTL_annunMACH_TRIM_FAIL"] = data.FCTL_annunMACH_TRIM_FAIL;
	js["FCTL_annunAUTO_SLAT_FAIL"] = data.FCTL_annunAUTO_SLAT_FAIL;
	js["FUEL_annunXFEED_VALVE_OPEN"] = data.FUEL_annunXFEED_VALVE_OPEN;
	js["ELEC_annunBAT_DISCHARGE"] = data.ELEC_annunBAT_DISCHARGE;
	js["ELEC_annunTR_UNIT"] = data.ELEC_annunTR_UNIT;
	js["ELEC_annunELEC"] = data.ELEC_annunELEC;
	js["ELEC_annunSTANDBY_POWER_OFF"] = data.ELEC_annunSTANDBY_POWER_OFF;
	js["ELEC_annunGRD_POWER_AVAILABLE"] = data.ELEC_annunGRD_POWER_AVAILABLE;
	js["ELEC_annunAPU_GEN_OFF_BUS"] = data.ELEC_annunAPU_GEN_OFF_BUS;
	js["APU_annunMAINT"] = data.APU_annunMAINT;
	js["APU_annunLOW_OIL_PRESSURE"] = data.APU_annunLOW_OIL_PRESSURE;
	js["APU_annunFAULT"] = data.APU_annunFAULT;
	js["APU_annunOVERSPEED"] = data.APU_annunOVERSPEED;
	js["AIR_annunEquipCoolingSupplyOFF"] = data.AIR_annunEquipCoolingSupplyOFF;
	js["AIR_annunEquipCoolingExhaustOFF"] = data.AIR_annunEquipCoolingExhaustOFF;
	js["LTS_annunEmerNOT_ARMED"] = data.LTS_annunEmerNOT_ARMED;
	js["COMM_annunCALL"] = data.COMM_annunCALL;
	js["COMM_annunPA_IN_USE"] = data.COMM_annunPA_IN_USE;
	js["ICE_annunCAPT_PITOT"] = data.ICE_annunCAPT_PITOT;
	js["ICE_annunL_ELEV_PITOT"] = data.ICE_annunL_ELEV_PITOT;
	js["ICE_annunL_ALPHA_VANE"] = data.ICE_annunL_ALPHA_VANE;
	js["ICE_annunL_TEMP_PROBE"] = data.ICE_annunL_TEMP_PROBE;
	js["ICE_annunFO_PITOT"] = data.ICE_annunFO_PITOT;
	js["ICE_annunR_ELEV_PITOT"] = data.ICE_annunR_ELEV_PITOT;
	js["ICE_annunR_ALPHA_VANE"] = data.ICE_annunR_ALPHA_VANE;
	js["ICE_annunAUX_PITOT"] = data.ICE_annunAUX_PITOT;
	js["AIR_annunDualBleed"] = data.AIR_annunDualBleed;
	js["AIR_annunRamDoorL"] = data.AIR_annunRamDoorL;
	js["AIR_annunRamDoorR"] = data.AIR_annunRamDoorR;
	js["AIR_FltAltWindow"] = data.AIR_FltAltWindow;
	js["AIR_LandAltWindow"] = data.AIR_LandAltWindow;
	js["WARN_annunFLT_CONT"] = data.WARN_annunFLT_CONT;
	js["WARN_annunIRS"] = data.WARN_annunIRS;
	js["WARN_annunFUEL"] = data.WARN_annunFUEL;
	js["WARN_annunELEC"] = data.WARN_annunELEC;
	js["WARN_annunAPU"] = data.WARN_annunAPU;
	js["WARN_annunOVHT_DET"] = data.WARN_annunOVHT_DET;
	js["WARN_annunANTI_ICE"] = data.WARN_annunANTI_ICE;
	js["WARN_annunHYD"] = data.WARN_annunHYD;
	js["WARN_annunDOORS"] = data.WARN_annunDOORS;
	js["WARN_annunENG"] = data.WARN_annunENG;
	js["WARN_annunOVERHEAD"] = data.WARN_annunOVERHEAD;
	js["WARN_annunAIR_COND"] = data.WARN_annunAIR_COND;
	js["MAIN_annunSPEEDBRAKE_ARMED"] = data.MAIN_annunSPEEDBRAKE_ARMED;
	js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM;
	js["MAIN_annunSPEEDBRAKE_EXTENDED"] = data.MAIN_annunSPEEDBRAKE_EXTENDED;
	js["MAIN_annunSTAB_OUT_OF_TRIM"] = data.MAIN_annunSTAB_OUT_OF_TRIM;
	js["MAIN_annunANTI_SKID_INOP"] = data.MAIN_annunANTI_SKID_INOP;
	js["MAIN_annunAUTO_BRAKE_DISARM"] = data.MAIN_annunAUTO_BRAKE_DISARM;
	js["MAIN_annunLE_FLAPS_TRANSIT"] = data.MAIN_annunLE_FLAPS_TRANSIT;
	js["MAIN_annunLE_FLAPS_EXT"] = data.MAIN_annunLE_FLAPS_EXT;
	js["HGS_annun_AIII"] = data.HGS_annun_AIII;
	js["HGS_annun_NO_AIII"] = data.HGS_annun_NO_AIII;
	js["HGS_annun_FLARE"] = data.HGS_annun_FLARE;
	js["HGS_annun_RO"] = data.HGS_annun_RO;
	js["HGS_annun_RO_CTN"] = data.HGS_annun_RO_CTN;
	js["HGS_annun_RO_ARM"] = data.HGS_annun_RO_ARM;
	js["HGS_annun_TO"] = data.HGS_annun_TO;
	js["HGS_annun_TO_CTN"] = data.HGS_annun_TO_CTN;
	js["HGS_annun_APCH"] = data.HGS_annun_APCH;
	js["HGS_annun_TO_WARN"] = data.HGS_annun_TO_WARN;
	js["HGS_annun_Bar"] = data.HGS_annun_Bar;
	js["HGS_annun_FAIL"] = data.HGS_annun_FAIL;
	js["GPWS_annunINOP"] = data.GPWS_annunINOP;
	js["PED_annunParkingBrake"] = data.PED_annunParkingBrake;
	js["FIRE_annunWHEEL_WELL"] = data.FIRE_annunWHEEL_WELL;
	js["FIRE_annunFAULT"] = data.FIRE_annunFAULT;
	js["FIRE_annunAPU_DET_INOP"] = data.FIRE_annunAPU_DET_INOP;
	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = data.FIRE_annunAPU_BOTTLE_DISCHARGE;
	js["CARGO_annunFWD"] = data.CARGO_annunFWD;
	js["CARGO_annunAFT"] = data.CARGO_annunAFT;
	js["CARGO_annunDETECTOR_FAULT"] = data.CARGO_annunDETECTOR_FAULT;
	js["CARGO_annunDISCH"] = data.CARGO_annunDISCH;
	js["HGS_annunRWY"] = data.HGS_annunRWY;
	js["HGS_annunGS"] = data.HGS_annunGS;
	js["HGS_annunFAULT"] = data.HGS_annunFAULT;
	js["HGS_annunCLR"] = data.HGS_annunCLR;
	js["XPDR_annunFAIL"] = data.XPDR_annunFAIL;
	js["PED_annunLOCK_FAIL"] = data.PED_annunLOCK_FAIL;
	js["PED_annunAUTO_UNLK"] = data.PED_annunAUTO_UNLK;
	js["DOOR_annunFWD_ENTRY"] = data.DOOR_annunFWD_ENTRY;
	js["DOOR_annunFWD_SERVICE"] = data.DOOR_annunFWD_SERVICE;
	js["DOOR_annunAIRSTAIR"] = data.DOOR_annunAIRSTAIR;
	js["DOOR_annunLEFT_FWD_OVERWING"] = data.DOOR_annunLEFT_FWD_OVERWING;
	js["DOOR_annunRIGHT_FWD_OVERWING"] = data.DOOR_annunRIGHT_FWD_OVERWING;
	js["DOOR_annunFWD_CARGO"] = data.DOOR_annunFWD_CARGO;
	js["DOOR_annunEQUIP"] = data.DOOR_annunEQUIP;
	js["DOOR_annunLEFT_AFT_OVERWING"] = data.DOOR_annunLEFT_AFT_OVERWING;
	js["DOOR_annunRIGHT_AFT_OVERWING"] = data.DOOR_annunRIGHT_AFT_OVERWING;
	js["DOOR_annunAFT_CARGO"] = data.DOOR_annunAFT_CARGO;
	js["DOOR_annunAFT_ENTRY"] = data.DOOR_annunAFT_ENTRY;
	js["DOOR_annunAFT_SERVICE"] = data.DOOR_annunAFT_SERVICE;
	js["AIR_annunAUTO_FAIL"] = data.AIR_annunAUTO_FAIL;
	js["AIR_annunOFFSCHED_DESCENT"] = data.AIR_annunOFFSCHED_DESCENT;
	js["AIR_annunALTN"] = data.AIR_annunALTN;
	js["AIR_annunMANUAL"] = data.AIR_annunMANUAL;
	js["IRS_annunALIGN_1"] = data.IRS_annunALIGN[0];
	js["IRS_annunALIGN_2"] = data.IRS_annunALIGN[1];
	js["IRS_annunON_DC_1"] = data.IRS_annunON_DC[0];
	js["IRS_annunON_DC_2"] = data.IRS_annunON_DC[1];
	js["IRS_annunFAULT_1"] = data.IRS_annunFAULT[0];
	js["IRS_annunFAULT_2"] = data.IRS_annunFAULT[1];
	js["IRS_annunDC_FAIL_1"] = data.IRS_annunDC_FAIL[0];
	js["IRS_annunDC_FAIL_2"] = data.IRS_annunDC_FAIL[1];
	js["ENG_annunREVERSER_1"] = data.ENG_annunREVERSER[0];
	js["ENG_annunREVERSER_2"] = data.ENG_annunREVERSER[1];
	js["ENG_annunENGINE_CONTROL_1"] = data.ENG_annunENGINE_CONTROL[0];
	js["ENG_annunENGINE_CONTROL_2"] = data.ENG_annunENGINE_CONTROL[1];
	js["ENG_annunALTN_1"] = data.ENG_annunALTN[0];
	js["ENG_annunALTN_2"] = data.ENG_annunALTN[1];
	js["FCTL_annunFC_LOW_PRESSURE_1"] = data.FCTL_annunFC_LOW_PRESSURE[0];
	js["FCTL_annunFC_LOW_PRESSURE_2"] = data.FCTL_annunFC_LOW_PRESSURE[1];
#ifdef PMDG_737_NG3 
	js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0] != 0;
	js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1] != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0] != 0;
	js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1] != 0;
#else
	js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0];
	js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1];
	js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0];
	js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1];
#endif
	js["FUEL_annunFILTER_BYPASS_1"] = data.FUEL_annunFILTER_BYPASS[0];
	js["FUEL_annunFILTER_BYPASS_2"] = data.FUEL_annunFILTER_BYPASS[1];
	js["FUEL_annunLOWPRESS_Fwd_1"] = data.FUEL_annunLOWPRESS_Fwd[0];
	js["FUEL_annunLOWPRESS_Fwd_2"] = data.FUEL_annunLOWPRESS_Fwd[1];
	js["FUEL_annunLOWPRESS_Aft_1"] = data.FUEL_annunLOWPRESS_Aft[0];
	js["FUEL_annunLOWPRESS_Aft_2"] = data.FUEL_annunLOWPRESS_Aft[1];
	js["FUEL_annunLOWPRESS_Ctr_1"] = data.FUEL_annunLOWPRESS_Ctr[0];
	js["FUEL_annunLOWPRESS_Ctr_2"] = data.FUEL_annunLOWPRESS_Ctr[1];
	js["ELEC_annunDRIVE_1"] = data.ELEC_annunDRIVE[0];
	js["ELEC_annunDRIVE_2"] = data.ELEC_annunDRIVE[1];
	js["ELEC_annunTRANSFER_BUS_OFF_1"] = data.ELEC_annunTRANSFER_BUS_OFF[0];
	js["ELEC_annunTRANSFER_BUS_OFF_2"] = data.ELEC_annunTRANSFER_BUS_OFF[1];
	js["ELEC_annunSOURCE_OFF_1"] = data.ELEC_annunSOURCE_OFF[0];
	js["ELEC_annunSOURCE_OFF_2"] = data.ELEC_annunSOURCE_OFF[1];
	js["ELEC_annunGEN_BUS_OFF_1"] = data.ELEC_annunGEN_BUS_OFF[0];
	js["ELEC_annunGEN_BUS_OFF_2"] = data.ELEC_annunGEN_BUS_OFF[1];
	js["ICE_annunOVERHEAT_1"] = data.ICE_annunOVERHEAT[0];
	js["ICE_annunOVERHEAT_2"] = data.ICE_annunOVERHEAT[1];
	js["ICE_annunOVERHEAT_3"] = data.ICE_annunOVERHEAT[2];
	js["ICE_annunOVERHEAT_4"] = data.ICE_annunOVERHEAT[3];
	js["ICE_annunON_1"] = data.ICE_annunON[0];
	js["ICE_annunON_2"] = data.ICE_annunON[1];
	js["ICE_annunON_3"] = data.ICE_annunON[2];
	js["ICE_annunON_4"] = data.ICE_annunON[3];
	js["ICE_annunVALVE_OPEN_1"] = data.ICE_annunVALVE_OPEN[0];
	js["ICE_annunVALVE_OPEN_2"] = data.ICE_annunVALVE_OPEN[1];
	js["ICE_annunCOWL_ANTI_ICE_1"] = data.ICE_annunCOWL_ANTI_ICE[0];
	js["ICE_annunCOWL_ANTI_ICE_2"] = data.ICE_annunCOWL_ANTI_ICE[1];
	js["ICE_annunCOWL_VALVE_OPEN_1"] = data.ICE_annunCOWL_VALVE_OPEN[0];
	js["ICE_annunCOWL_VALVE_OPEN_2"] = data.ICE_annunCOWL_VALVE_OPEN[1];
	js["HYD_annunLOW_PRESS_eng_1"] = data.HYD_annunLOW_PRESS_eng[0];
	js["HYD_annunLOW_PRESS_eng_2"] = data.HYD_annunLOW_PRESS_eng[1];
	js["HYD_annunLOW_PRESS_elec_1"] = data.HYD_annunLOW_PRESS_elec[0];
	js["HYD_annunLOW_PRESS_elec_2"] = data.HYD_annunLOW_PRESS_elec[1];
	js["HYD_annunOVERHEAT_elec_1"] = data.HYD_annunOVERHEAT_elec[0];
	js["HYD_annunOVERHEAT_elec_2"] = data.HYD_annunOVERHEAT_elec[1];
	js["AIR_annunZoneTemp_1"] = data.AIR_annunZoneTemp[0];
	js["AIR_annunZoneTemp_2"] = data.AIR_annunZoneTemp[1];
	js["AIR_annunZoneTemp_3"] = data.AIR_annunZoneTemp[2];
	js["AIR_annunPackTripOff_1"] = data.AIR_annunPackTripOff[0];
	js["AIR_annunPackTripOff_2"] = data.AIR_annunPackTripOff[1];
	js["AIR_annunWingBodyOverheat_1"] = data.AIR_annunWingBodyOverheat[0];
	js["AIR_annunWingBodyOverheat_2"] = data.AIR_annunWingBodyOverheat[1];
	js["AIR_annunBleedTripOff_1"] = data.AIR_annunBleedTripOff[0];
	js["AIR_annunBleedTripOff_2"] = data.AIR_annunBleedTripOff[1];
	js["WARN_annunFIRE_WARN_1"] = data.WARN_annunFIRE_WARN[0];
	js["WARN_annunFIRE_WARN_2"] = data.WARN_annunFIRE_WARN[1];
	js["WARN_annunMASTER_CAUTION_1"] = data.WARN_annunMASTER_CAUTION[0];
	js["WARN_annunMASTER_CAUTION_2"] = data.WARN_annunMASTER_CAUTION[1];
	js["MAIN_annunBELOW_GS_1"] = data.MAIN_annunBELOW_GS[0];
	js["MAIN_annunBELOW_GS_2"] = data.MAIN_annunBELOW_GS[1];
	js["MAIN_annunAP_1"] = data.MAIN_annunAP[0];
	js["MAIN_annunAP_2"] = data.MAIN_annunAP[1];
	js["MAIN_annunAT_1"] = data.MAIN_annunAT[0];
	js["MAIN_annunAT_2"] = data.MAIN_annunAT[1];
	js["MAIN_annunFMC_1"] = data.MAIN_annunFMC[0];
	js["MAIN_annunFMC_2"] = data.MAIN_annunFMC[1];
	js["MAIN_annunGEAR_transit_1"] = data.MAIN_annunGEAR_transit[0];
	js["MAIN_annunGEAR_transit_2"] = data.MAIN_annunGEAR_transit[1];
	js["MAIN_annunGEAR_transit_3"] = data.MAIN_annunGEAR_transit[2];
	js["MAIN_annunGEAR_locked_1"] = data.MAIN_annunGEAR_locked[0];
	js["MAIN_annunGEAR_locked_2"] = data.MAIN_annunGEAR_locked[1];
	js["MAIN_annunGEAR_locked_3"] = data.MAIN_annunGEAR_locked[2];
	js["FIRE_annunENG_OVERHEAT_1"] = data.FIRE_annunENG_OVERHEAT[0];
	js["FIRE_annunENG_OVERHEAT_2"] = data.FIRE_annunENG_OVERHEAT[1];
	js["FIRE_annunBOTTLE_DISCHARGE_1"] = data.FIRE_annunBOTTLE_DISCHARGE[0];
	js["FIRE_annunBOTTLE_DISCHARGE_2"] = data.FIRE_annunBOTTLE_DISCHARGE[1];
	js["FIRE_annunExtinguisherTest_1"] = data.FIRE_annunExtinguisherTest[0];
	js["FIRE_annunExtinguisherTest_2"] = data.FIRE_annunExtinguisherTest[1];
	js["FIRE_annunExtinguisherTest_3"] = data.FIRE_annunExtinguisherTest[2];
	js["CARGO_annunExtTest_1"] = data.CARGO_annunExtTest[0];
	js["CARGO_annunExtTest_2"] = data.CARGO_annunExtTest[1];
	js["MAIN_annunAP_Amber_1"] = data.MAIN_annunAP_Amber[0];
	js["MAIN_annunAP_Amber_2"] = data.MAIN_annunAP_Amber[1];
	js["MAIN_annunAT_Amber_1"] = data.MAIN_annunAT_Amber[0];
	js["MAIN_annunAT_Amber_2"] = data.MAIN_annunAT_Amber[1];
	js["MCP_FDSw_1"] = data.MCP_FDSw[0];
	js["MCP_FDSw_2"] = data.MCP_FDSw[1];
	js["EFIS_ModeSel_1"] = data.EFIS_ModeSel[0];
	js["EFIS_ModeSel_2"] = data.EFIS_ModeSel[1];
	js["MCP_DisengageBar"] = data.MCP_DisengageBar;

	return js.dump();
}

std::string B737ngx::buildJsonPmdg737(const AIRCRAFT_DATA_TYPE & data)
{
	json js;

	if (ngxData.MCP_DisengageBar != data.MCP_DisengageBar) {
		ngxData.MCP_DisengageBar = data.MCP_DisengageBar;
		js["MCP_DisengageBar"] = data.MCP_DisengageBar;
	}
	if(ngxData.EFIS_ModeSel[0] != data.EFIS_ModeSel[0])
	{
		ngxData.EFIS_ModeSel[0] = data.EFIS_ModeSel[0];
		js["EFIS_ModeSel_1"] = data.EFIS_ModeSel[0];
	}
	if (ngxData.EFIS_ModeSel[1] != data.EFIS_ModeSel[1])
	{
		ngxData.EFIS_ModeSel[1] = data.EFIS_ModeSel[1];
		js["EFIS_ModeSel_2"] = data.EFIS_ModeSel[1];
	}
	if (ngxData.MCP_FDSw[0] != data.MCP_FDSw[0]) {
		ngxData.MCP_FDSw[0] = data.MCP_FDSw[0];
		js["MCP_FDSw_1"] = data.MCP_FDSw[0];
	}
	if (ngxData.MCP_FDSw[1] != data.MCP_FDSw[1]) {
		ngxData.MCP_FDSw[1] = data.MCP_FDSw[1];
		js["MCP_FDSw_2"] = data.MCP_FDSw[1];
	}

	if( abs(ngxData.OXY_Needle  - data.OXY_Needle) > 20)
	{
		ngxData.OXY_Needle = data.OXY_Needle;
		js["OXY_Needle"] = data.OXY_Needle;
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
	//	js["COMM_ServiceInterphoneSw"] = data.COMM_ServiceInterphoneSw;
	//}
	if (ngxData.OXY_SwNormal != data.OXY_SwNormal) {
		ngxData.OXY_SwNormal = data.OXY_SwNormal;
		js["OXY_SwNormal"] = data.OXY_SwNormal;
	}
	if (ngxData.FLTREC_SwNormal != data.FLTREC_SwNormal) {
		ngxData.FLTREC_SwNormal = data.FLTREC_SwNormal;
#ifdef PMDG_737_NG3 
		js["FLTREC_SwNormal"] = !data.FLTREC_SwNormal;
#else
		js["FLTREC_SwNormal"] = data.FLTREC_SwNormal;
#endif
	}
	if (ngxData.FCTL_YawDamper_Sw != data.FCTL_YawDamper_Sw) {
		ngxData.FCTL_YawDamper_Sw = data.FCTL_YawDamper_Sw;
		js["FCTL_YawDamper_Sw"] = data.FCTL_YawDamper_Sw;
	}
	if (ngxData.FCTL_AltnFlaps_Sw_ARM != data.FCTL_AltnFlaps_Sw_ARM) {
		ngxData.FCTL_AltnFlaps_Sw_ARM = data.FCTL_AltnFlaps_Sw_ARM;
		js["FCTL_AltnFlaps_Sw_ARM"] = data.FCTL_AltnFlaps_Sw_ARM;
	}
	if (ngxData.FCTL_AltnFlaps_Control_Sw != data.FCTL_AltnFlaps_Control_Sw) {
		ngxData.FCTL_AltnFlaps_Control_Sw = data.FCTL_AltnFlaps_Control_Sw;
		js["FCTL_AltnFlaps_Control_Sw"] = data.FCTL_AltnFlaps_Control_Sw;
	}
	if (ngxData.NAVDIS_VHFNavSelector != data.NAVDIS_VHFNavSelector) {
		ngxData.NAVDIS_VHFNavSelector = data.NAVDIS_VHFNavSelector;
		js["NAVDIS_VHFNavSelector"] = data.NAVDIS_VHFNavSelector;
	}
	if (ngxData.NAVDIS_IRSSelector != data.NAVDIS_IRSSelector) {
		ngxData.NAVDIS_IRSSelector = data.NAVDIS_IRSSelector;
		js["NAVDIS_IRSSelector"] = data.NAVDIS_IRSSelector;
	}
	if (ngxData.NAVDIS_FMCSelector != data.NAVDIS_FMCSelector) {
		ngxData.NAVDIS_FMCSelector = data.NAVDIS_FMCSelector;
		js["NAVDIS_FMCSelector"] = data.NAVDIS_FMCSelector;
	}
	if (ngxData.NAVDIS_SourceSelector != data.NAVDIS_SourceSelector) {
		ngxData.NAVDIS_SourceSelector = data.NAVDIS_SourceSelector;
		js["NAVDIS_SourceSelector"] = data.NAVDIS_SourceSelector;
	}
	if (ngxData.NAVDIS_ControlPaneSelector != data.NAVDIS_ControlPaneSelector) {
		ngxData.NAVDIS_ControlPaneSelector = data.NAVDIS_ControlPaneSelector;
		js["NAVDIS_ControlPaneSelector"] = data.NAVDIS_ControlPaneSelector;
	}
	if (ngxData.FUEL_CrossFeedSw != data.FUEL_CrossFeedSw) {
		ngxData.FUEL_CrossFeedSw = data.FUEL_CrossFeedSw;
		js["FUEL_CrossFeedSw"] = data.FUEL_CrossFeedSw;
	}
	if (ngxData.ELEC_DCMeterSelector != data.ELEC_DCMeterSelector) {
		ngxData.ELEC_DCMeterSelector = data.ELEC_DCMeterSelector;
		js["ELEC_DCMeterSelector"] = data.ELEC_DCMeterSelector;
	}
	//if (ngxData.ELEC_ACMeterSelector != data.ELEC_ACMeterSelector) {
	//	ngxData.ELEC_ACMeterSelector = data.ELEC_ACMeterSelector;
	//	js["ELEC_ACMeterSelector"] = data.ELEC_ACMeterSelector;
	//}
	if (ngxData.ELEC_BatSelector != data.ELEC_BatSelector) {
		ngxData.ELEC_BatSelector = data.ELEC_BatSelector;
		js["ELEC_BatSelector"] = data.ELEC_BatSelector;
	}
	if (ngxData.ELEC_CabUtilSw != data.ELEC_CabUtilSw) {
		ngxData.ELEC_CabUtilSw = data.ELEC_CabUtilSw;
		js["ELEC_CabUtilSw"] = data.ELEC_CabUtilSw;
	}
	if (ngxData.ELEC_IFEPassSeatSw != data.ELEC_IFEPassSeatSw) {
		ngxData.ELEC_IFEPassSeatSw = data.ELEC_IFEPassSeatSw;
		js["ELEC_IFEPassSeatSw"] = data.ELEC_IFEPassSeatSw;
	}
	if (ngxData.ELEC_StandbyPowerSelector != data.ELEC_StandbyPowerSelector) {
		ngxData.ELEC_StandbyPowerSelector = data.ELEC_StandbyPowerSelector;
		js["ELEC_StandbyPowerSelector"] = data.ELEC_StandbyPowerSelector;
	}
	if (ngxData.ELEC_GrdPwrSw != data.ELEC_GrdPwrSw) {
		ngxData.ELEC_GrdPwrSw = data.ELEC_GrdPwrSw;
		js["ELEC_GrdPwrSw"] = data.ELEC_GrdPwrSw;
	}
	if (ngxData.ELEC_BusTransSw_AUTO != data.ELEC_BusTransSw_AUTO) {
		ngxData.ELEC_BusTransSw_AUTO = data.ELEC_BusTransSw_AUTO;
		js["ELEC_BusTransSw_AUTO"] = data.ELEC_BusTransSw_AUTO;
	}
	if (ngxData.OH_WiperLSelector != data.OH_WiperLSelector) {
		ngxData.OH_WiperLSelector = data.OH_WiperLSelector;
		js["OH_WiperLSelector"] = data.OH_WiperLSelector;
	}
	if (ngxData.OH_WiperRSelector != data.OH_WiperRSelector) {
		ngxData.OH_WiperRSelector = data.OH_WiperRSelector;
		js["OH_WiperRSelector"] = data.OH_WiperRSelector;
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
	if (ngxData.AIR_EquipCoolingSupplyNORM != data.AIR_EquipCoolingSupplyNORM) {
		ngxData.AIR_EquipCoolingSupplyNORM = data.AIR_EquipCoolingSupplyNORM;
		js["AIR_EquipCoolingSupplyNORM"] = data.AIR_EquipCoolingSupplyNORM;
	}
	if (ngxData.AIR_EquipCoolingExhaustNORM != data.AIR_EquipCoolingExhaustNORM) {
		ngxData.AIR_EquipCoolingExhaustNORM = data.AIR_EquipCoolingExhaustNORM;
		js["AIR_EquipCoolingExhaustNORM"] = data.AIR_EquipCoolingExhaustNORM;
	}
	if (ngxData.LTS_EmerExitSelector != data.LTS_EmerExitSelector) {
		ngxData.LTS_EmerExitSelector = data.LTS_EmerExitSelector;
		js["LTS_EmerExitSelector"] = data.LTS_EmerExitSelector;
	}
	//if (ngxData.COMM_NoSmokingSelector != data.COMM_NoSmokingSelector) {
	//	ngxData.COMM_NoSmokingSelector = data.COMM_NoSmokingSelector;
	//	js["COMM_NoSmokingSelector"] = data.COMM_NoSmokingSelector;
	//}
	if (ngxData.COMM_FastenBeltsSelector != data.COMM_FastenBeltsSelector) {
		ngxData.COMM_FastenBeltsSelector = data.COMM_FastenBeltsSelector;
		js["COMM_FastenBeltsSelector"] = data.COMM_FastenBeltsSelector;
	}
	if (ngxData.ICE_WingAntiIceSw != data.ICE_WingAntiIceSw) {
		ngxData.ICE_WingAntiIceSw = data.ICE_WingAntiIceSw;
		js["ICE_WingAntiIceSw"] = data.ICE_WingAntiIceSw;
	}
	//if (ngxData.AIR_TempSourceSelector != data.AIR_TempSourceSelector) {
	//	ngxData.AIR_TempSourceSelector = data.AIR_TempSourceSelector;
	//	js["AIR_TempSourceSelector"] = data.AIR_TempSourceSelector;
	//}
	if (ngxData.AIR_TrimAirSwitch != data.AIR_TrimAirSwitch) {
		ngxData.AIR_TrimAirSwitch = data.AIR_TrimAirSwitch;
		js["AIR_TrimAirSwitch"] = data.AIR_TrimAirSwitch;
	}
	if (ngxData.AIR_APUBleedAirSwitch != data.AIR_APUBleedAirSwitch) {
		ngxData.AIR_APUBleedAirSwitch = data.AIR_APUBleedAirSwitch;
		js["AIR_APUBleedAirSwitch"] = data.AIR_APUBleedAirSwitch;
	}
	if (ngxData.AIR_IsolationValveSwitch != data.AIR_IsolationValveSwitch) {
		ngxData.AIR_IsolationValveSwitch = data.AIR_IsolationValveSwitch;
		js["AIR_IsolationValveSwitch"] = data.AIR_IsolationValveSwitch;
	}
	//if (ngxData.AIR_OutflowValveSwitch != data.AIR_OutflowValveSwitch) {
	//	ngxData.AIR_OutflowValveSwitch = data.AIR_OutflowValveSwitch;
	//	js["AIR_OutflowValveSwitch"] = data.AIR_OutflowValveSwitch;
	//}
	if (ngxData.AIR_PressurizationModeSelector != data.AIR_PressurizationModeSelector) {
		ngxData.AIR_PressurizationModeSelector = data.AIR_PressurizationModeSelector;
		js["AIR_PressurizationModeSelector"] = data.AIR_PressurizationModeSelector;
	}
	if (ngxData.LTS_TaxiSw != data.LTS_TaxiSw) {
		ngxData.LTS_TaxiSw = data.LTS_TaxiSw;
		js["LTS_TaxiSw"] = data.LTS_TaxiSw;
	}
	if (ngxData.APU_Selector != data.APU_Selector) {
		ngxData.APU_Selector = data.APU_Selector;
		js["APU_Selector"] = data.APU_Selector;
	}
	if (ngxData.ENG_IgnitionSelector != data.ENG_IgnitionSelector) {
		ngxData.ENG_IgnitionSelector = data.ENG_IgnitionSelector;
		js["ENG_IgnitionSelector"] = data.ENG_IgnitionSelector;
	}
	//if (ngxData.LTS_LogoSw != data.LTS_LogoSw) {
	//	ngxData.LTS_LogoSw = data.LTS_LogoSw;
	//	js["LTS_LogoSw"] = data.LTS_LogoSw;
	//}
	if (ngxData.LTS_PositionSw != data.LTS_PositionSw) {
		ngxData.LTS_PositionSw = data.LTS_PositionSw;
		js["LTS_PositionSw"] = data.LTS_PositionSw;
	}
	if (ngxData.LTS_AntiCollisionSw != data.LTS_AntiCollisionSw) {
		ngxData.LTS_AntiCollisionSw = data.LTS_AntiCollisionSw;
		js["LTS_AntiCollisionSw"] = data.LTS_AntiCollisionSw;
	}
	if (ngxData.LTS_WingSw != data.LTS_WingSw) {
		ngxData.LTS_WingSw = data.LTS_WingSw;
		js["LTS_WingSw"] = data.LTS_WingSw;
	}
	if (ngxData.LTS_WheelWellSw != data.LTS_WheelWellSw) {
		ngxData.LTS_WheelWellSw = data.LTS_WheelWellSw;
		js["LTS_WheelWellSw"] = data.LTS_WheelWellSw;
	}
	if (ngxData.MAIN_NoseWheelSteeringSwNORM != data.MAIN_NoseWheelSteeringSwNORM) {
		ngxData.MAIN_NoseWheelSteeringSwNORM = data.MAIN_NoseWheelSteeringSwNORM;
		js["MAIN_NoseWheelSteeringSwNORM"] = data.MAIN_NoseWheelSteeringSwNORM;
	}
	//if (ngxData.MAIN_LightsSelector != data.MAIN_LightsSelector) {
	//	ngxData.MAIN_LightsSelector = data.MAIN_LightsSelector;
	//	js["MAIN_LightsSelector"] = data.MAIN_LightsSelector;
	//}
	//if (ngxData.MAIN_RMISelector1_VOR != data.MAIN_RMISelector1_VOR) {
	//	ngxData.MAIN_RMISelector1_VOR = data.MAIN_RMISelector1_VOR;
	//	js["MAIN_RMISelector1_VOR"] = data.MAIN_RMISelector1_VOR;
	//}
	//if (ngxData.MAIN_RMISelector2_VOR != data.MAIN_RMISelector2_VOR) {
	//	ngxData.MAIN_RMISelector2_VOR = data.MAIN_RMISelector2_VOR;
	//	js["MAIN_RMISelector2_VOR"] = data.MAIN_RMISelector2_VOR;
	//}
	if (ngxData.MAIN_N1SetSelector != data.MAIN_N1SetSelector) {
		ngxData.MAIN_N1SetSelector = data.MAIN_N1SetSelector;
		js["MAIN_N1SetSelector"] = data.MAIN_N1SetSelector;
	}
	if (ngxData.MAIN_SpdRefSelector != data.MAIN_SpdRefSelector) {
		ngxData.MAIN_SpdRefSelector = data.MAIN_SpdRefSelector;
		js["MAIN_SpdRefSelector"] = data.MAIN_SpdRefSelector;
	}
	if (ngxData.MAIN_FuelFlowSelector != data.MAIN_FuelFlowSelector) {
		ngxData.MAIN_FuelFlowSelector = data.MAIN_FuelFlowSelector;
		js["MAIN_FuelFlowSelector"] = data.MAIN_FuelFlowSelector;
	}
	if (ngxData.MAIN_AutobrakeSelector != data.MAIN_AutobrakeSelector) {
		ngxData.MAIN_AutobrakeSelector = data.MAIN_AutobrakeSelector;
		js["MAIN_AutobrakeSelector"] = data.MAIN_AutobrakeSelector;
	}
	if (ngxData.MAIN_GearLever != data.MAIN_GearLever) {
		ngxData.MAIN_GearLever = data.MAIN_GearLever;
		js["MAIN_GearLever"] = data.MAIN_GearLever;
	}
	if (ngxData.GPWS_FlapInhibitSw_NORM != data.GPWS_FlapInhibitSw_NORM) {
		ngxData.GPWS_FlapInhibitSw_NORM = data.GPWS_FlapInhibitSw_NORM;
		js["GPWS_FlapInhibitSw_NORM"] = data.GPWS_FlapInhibitSw_NORM;
	}
	if (ngxData.GPWS_GearInhibitSw_NORM != data.GPWS_GearInhibitSw_NORM) {
		ngxData.GPWS_GearInhibitSw_NORM = data.GPWS_GearInhibitSw_NORM;
		js["GPWS_GearInhibitSw_NORM"] = data.GPWS_GearInhibitSw_NORM;
	}
	if (ngxData.GPWS_TerrInhibitSw_NORM != data.GPWS_TerrInhibitSw_NORM) {
		ngxData.GPWS_TerrInhibitSw_NORM = data.GPWS_TerrInhibitSw_NORM;
		js["GPWS_TerrInhibitSw_NORM"] = data.GPWS_TerrInhibitSw_NORM;
	}
	if (ngxData.TRIM_StabTrimMainElecSw_NORMAL != data.TRIM_StabTrimMainElecSw_NORMAL) {
		ngxData.TRIM_StabTrimMainElecSw_NORMAL = data.TRIM_StabTrimMainElecSw_NORMAL;
		js["TRIM_StabTrimMainElecSw_NORMAL"] = data.TRIM_StabTrimMainElecSw_NORMAL;
	}
	if (ngxData.TRIM_StabTrimAutoPilotSw_NORMAL != data.TRIM_StabTrimAutoPilotSw_NORMAL) {
		ngxData.TRIM_StabTrimAutoPilotSw_NORMAL = data.TRIM_StabTrimAutoPilotSw_NORMAL;
		js["TRIM_StabTrimAutoPilotSw_NORMAL"] = data.TRIM_StabTrimAutoPilotSw_NORMAL;
	}
	//if (ngxData.FIRE_DetTestSw != data.FIRE_DetTestSw) {
	//	ngxData.FIRE_DetTestSw = data.FIRE_DetTestSw;
	//	js["FIRE_DetTestSw"] = data.FIRE_DetTestSw;
	//}
	//if (ngxData.FIRE_ExtinguisherTestSw != data.FIRE_ExtinguisherTestSw) {
	//	ngxData.FIRE_ExtinguisherTestSw = data.FIRE_ExtinguisherTestSw;
	//	js["FIRE_ExtinguisherTestSw"] = data.FIRE_ExtinguisherTestSw;
	//}
	//if (ngxData.XPDR_XpndrSelector_2 != data.XPDR_XpndrSelector_2) {
	//	ngxData.XPDR_XpndrSelector_2 = data.XPDR_XpndrSelector_2;
	//	js["XPDR_XpndrSelector_2"] = data.XPDR_XpndrSelector_2;
	//}
	//if (ngxData.XPDR_AltSourceSel_2 != data.XPDR_AltSourceSel_2) {
	//	ngxData.XPDR_AltSourceSel_2 = data.XPDR_AltSourceSel_2;
	//	js["XPDR_AltSourceSel_2"] = data.XPDR_AltSourceSel_2;
	//}
	//if (ngxData.XPDR_ModeSel != data.XPDR_ModeSel) {
	//	ngxData.XPDR_ModeSel = data.XPDR_ModeSel;
	//	js["XPDR_ModeSel"] = data.XPDR_ModeSel;
	//}
	if (ngxData.TRIM_StabTrimSw_NORMAL != data.TRIM_StabTrimSw_NORMAL) {
		ngxData.TRIM_StabTrimSw_NORMAL = data.TRIM_StabTrimSw_NORMAL;
		js["TRIM_StabTrimSw_NORMAL"] = data.TRIM_StabTrimSw_NORMAL;
	}
	//if (ngxData.PED_FltDkDoorSel != data.PED_FltDkDoorSel) {
	//	ngxData.PED_FltDkDoorSel = data.PED_FltDkDoorSel;
	//	js["PED_FltDkDoorSel"] = data.PED_FltDkDoorSel;
	//}
	if (ngxData.IRS_aligned != data.IRS_aligned) {
		ngxData.IRS_aligned = data.IRS_aligned;
		js["IRS_aligned"] = data.IRS_aligned;
	}
	//if (ngxData.AircraftModel != data.AircraftModel) {
	//	ngxData.AircraftModel = data.AircraftModel;
	//	js["AircraftModel"] = data.AircraftModel;
	//}
	//if (ngxData.WeightInKg != data.WeightInKg) {
	//	ngxData.WeightInKg = data.WeightInKg;
	//	js["WeightInKg"] = data.WeightInKg;
	//}
	//if (ngxData.GPWS_V1CallEnabled != data.GPWS_V1CallEnabled) {
	//	ngxData.GPWS_V1CallEnabled = data.GPWS_V1CallEnabled;
	//	js["GPWS_V1CallEnabled"] = data.GPWS_V1CallEnabled;
	//}
	//if (ngxData.GroundConnAvailable != data.GroundConnAvailable) {
	//	ngxData.GroundConnAvailable = data.GroundConnAvailable;
	//	js["GroundConnAvailable"] = data.GroundConnAvailable;
	//}
	//if (ngxData.FMC_TakeoffFlaps != data.FMC_TakeoffFlaps) {
	//	ngxData.FMC_TakeoffFlaps = data.FMC_TakeoffFlaps;
	//	js["FMC_TakeoffFlaps"] = data.FMC_TakeoffFlaps;
	//}
	//if (ngxData.FMC_V1 != data.FMC_V1) {
	//	ngxData.FMC_V1 = data.FMC_V1;
	//	js["FMC_V1"] = data.FMC_V1;
	//}
	//if (ngxData.FMC_VR != data.FMC_VR) {
	//	ngxData.FMC_VR = data.FMC_VR;
	//	js["FMC_VR"] = data.FMC_VR;
	//}
	//if (ngxData.FMC_V2 != data.FMC_V2) {
	//	ngxData.FMC_V2 = data.FMC_V2;
	//	js["FMC_V2"] = data.FMC_V2;
	//}
	//if (ngxData.FMC_LandingFlaps != data.FMC_LandingFlaps) {
	//	ngxData.FMC_LandingFlaps = data.FMC_LandingFlaps;
	//	js["FMC_LandingFlaps"] = data.FMC_LandingFlaps;
	//}
	//if (ngxData.FMC_LandingVREF != data.FMC_LandingVREF) {
	//	ngxData.FMC_LandingVREF = data.FMC_LandingVREF;
	//	js["FMC_LandingVREF"] = data.FMC_LandingVREF;
	//}
	//if (ngxData.FMC_CruiseAlt != data.FMC_CruiseAlt) {
	//	ngxData.FMC_CruiseAlt = data.FMC_CruiseAlt;
	//	js["FMC_CruiseAlt"] = data.FMC_CruiseAlt;
	//}
	//if (ngxData.FMC_LandingAltitude != data.FMC_LandingAltitude) {
	//	ngxData.FMC_LandingAltitude = data.FMC_LandingAltitude;
	//	js["FMC_LandingAltitude"] = data.FMC_LandingAltitude;
	//}
	//if (ngxData.FMC_TransitionAlt != data.FMC_TransitionAlt) {
	//	ngxData.FMC_TransitionAlt = data.FMC_TransitionAlt;
	//	js["FMC_TransitionAlt"] = data.FMC_TransitionAlt;
	//}
	//if (ngxData.FMC_TransitionLevel != data.FMC_TransitionLevel) {
	//	ngxData.FMC_TransitionLevel = data.FMC_TransitionLevel;
	//	js["FMC_TransitionLevel"] = data.FMC_TransitionLevel;
	//}
	//if (ngxData.FMC_PerfInputComplete != data.FMC_PerfInputComplete) {
	//	ngxData.FMC_PerfInputComplete = data.FMC_PerfInputComplete;
	//	js["FMC_PerfInputComplete"] = data.FMC_PerfInputComplete;
	//}
	//if (ngxData.FMC_DistanceToTOD != data.FMC_DistanceToTOD) {
	//	ngxData.FMC_DistanceToTOD = data.FMC_DistanceToTOD;
	//	js["FMC_DistanceToTOD"] = data.FMC_DistanceToTOD;
	//}
	//if (ngxData.FMC_DistanceToDest != data.FMC_DistanceToDest) {
	//	ngxData.FMC_DistanceToDest = data.FMC_DistanceToDest;
	//	js["FMC_DistanceToDest"] = data.FMC_DistanceToDest;
	//}
	//if (ngxData.ICE_WindowHeatTestSw != data.ICE_WindowHeatTestSw) {
	//	ngxData.ICE_WindowHeatTestSw = data.ICE_WindowHeatTestSw;
	//	js["ICE_WindowHeatTestSw"] = data.ICE_WindowHeatTestSw;
	//}


	if (ngxData.IRS_ModeSelector[0] != data.IRS_ModeSelector[0]) {
		ngxData.IRS_ModeSelector[0] = data.IRS_ModeSelector[0];
		js["IRS_ModeSelector_1"] = data.IRS_ModeSelector[0];
	}
	if (ngxData.IRS_ModeSelector[1] != data.IRS_ModeSelector[1]) {
		ngxData.IRS_ModeSelector[1] = data.IRS_ModeSelector[1];
		js["IRS_ModeSelector_2"] = data.IRS_ModeSelector[1];
	}
	if (ngxData.ENG_EECSwitch[0] != data.ENG_EECSwitch[0]) {
		ngxData.ENG_EECSwitch[0] = data.ENG_EECSwitch[0];
		js["ENG_EECSwitch_1"] = data.ENG_EECSwitch[0];
	}
	if (ngxData.ENG_EECSwitch[1] != data.ENG_EECSwitch[1]) {
		ngxData.ENG_EECSwitch[1] = data.ENG_EECSwitch[1];
		js["ENG_EECSwitch_2"] = data.ENG_EECSwitch[1];
	}
	if (ngxData.FCTL_FltControl_Sw[0] != data.FCTL_FltControl_Sw[0]) {
		ngxData.FCTL_FltControl_Sw[0] = data.FCTL_FltControl_Sw[0];
		js["FCTL_FltControl_Sw_1"] = data.FCTL_FltControl_Sw[0];
	}
	if (ngxData.FCTL_FltControl_Sw[1] != data.FCTL_FltControl_Sw[1]) {
		ngxData.FCTL_FltControl_Sw[1] = data.FCTL_FltControl_Sw[1];
		js["FCTL_FltControl_Sw_2"] = data.FCTL_FltControl_Sw[1];
	}
	if (ngxData.FCTL_Spoiler_Sw[0] != data.FCTL_Spoiler_Sw[0]) {
		ngxData.FCTL_Spoiler_Sw[0] = data.FCTL_Spoiler_Sw[0];
		js["FCTL_Spoiler_Sw_1"] = data.FCTL_Spoiler_Sw[0];
	}
	if (ngxData.FCTL_Spoiler_Sw[1] != data.FCTL_Spoiler_Sw[1]) {
		ngxData.FCTL_Spoiler_Sw[1] = data.FCTL_Spoiler_Sw[1];
		js["FCTL_Spoiler_Sw_2"] = data.FCTL_Spoiler_Sw[1];
	}
	if (ngxData.FUEL_PumpFwdSw[0] != data.FUEL_PumpFwdSw[0]) {
		ngxData.FUEL_PumpFwdSw[0] = data.FUEL_PumpFwdSw[0];
		js["FUEL_PumpFwdSw_1"] = data.FUEL_PumpFwdSw[0];
	}
	if (ngxData.FUEL_PumpFwdSw[1] != data.FUEL_PumpFwdSw[1]) {
		ngxData.FUEL_PumpFwdSw[1] = data.FUEL_PumpFwdSw[1];
		js["FUEL_PumpFwdSw_2"] = data.FUEL_PumpFwdSw[1];
	}
	if (ngxData.FUEL_PumpAftSw[0] != data.FUEL_PumpAftSw[0]) {
		ngxData.FUEL_PumpAftSw[0] = data.FUEL_PumpAftSw[0];
		js["FUEL_PumpAftSw_1"] = data.FUEL_PumpAftSw[0];
	}
	if (ngxData.FUEL_PumpAftSw[1] != data.FUEL_PumpAftSw[1]) {
		ngxData.FUEL_PumpAftSw[1] = data.FUEL_PumpAftSw[1];
		js["FUEL_PumpAftSw_2"] = data.FUEL_PumpAftSw[1];
	}
	if (ngxData.FUEL_PumpCtrSw[0] != data.FUEL_PumpCtrSw[0]) {
		ngxData.FUEL_PumpCtrSw[0] = data.FUEL_PumpCtrSw[0];
		js["FUEL_PumpCtrSw_1"] = data.FUEL_PumpCtrSw[0];
	}
	if (ngxData.FUEL_PumpCtrSw[1] != data.FUEL_PumpCtrSw[1]) {
		ngxData.FUEL_PumpCtrSw[1] = data.FUEL_PumpCtrSw[1];
		js["FUEL_PumpCtrSw_2"] = data.FUEL_PumpCtrSw[1];
	}
	if (ngxData.ELEC_IDGDisconnectSw[0] != data.ELEC_IDGDisconnectSw[0]) {
		ngxData.ELEC_IDGDisconnectSw[0] = data.ELEC_IDGDisconnectSw[0];
		js["ELEC_IDGDisconnectSw_1"] = data.ELEC_IDGDisconnectSw[0];
	}
	if (ngxData.ELEC_IDGDisconnectSw[1] != data.ELEC_IDGDisconnectSw[1]) {
		ngxData.ELEC_IDGDisconnectSw[1] = data.ELEC_IDGDisconnectSw[1];
		js["ELEC_IDGDisconnectSw_2"] = data.ELEC_IDGDisconnectSw[1];
	}
	//if (ngxData.ELEC_GenSw[0] != data.ELEC_GenSw[0]) {
	//	ngxData.ELEC_GenSw[0] = data.ELEC_GenSw[0];
	//	js["ELEC_GenSw_1"] = data.ELEC_GenSw[0];
	//}
	//if (ngxData.ELEC_GenSw[1] != data.ELEC_GenSw[1]) {
	//	ngxData.ELEC_GenSw[1] = data.ELEC_GenSw[1];
	//	js["ELEC_GenSw_2"] = data.ELEC_GenSw[1];
	//}
	if (ngxData.ELEC_APUGenSw[0] != data.ELEC_APUGenSw[0]) {
		ngxData.ELEC_APUGenSw[0] = data.ELEC_APUGenSw[0];
		js["ELEC_APUGenSw_1"] = data.ELEC_APUGenSw[0];
	}
	if (ngxData.ELEC_APUGenSw[1] != data.ELEC_APUGenSw[1]) {
		ngxData.ELEC_APUGenSw[1] = data.ELEC_APUGenSw[1];
		js["ELEC_APUGenSw_2"] = data.ELEC_APUGenSw[1];
	}
	if (ngxData.ICE_WindowHeatSw[0] != data.ICE_WindowHeatSw[0]) {
		ngxData.ICE_WindowHeatSw[0] = data.ICE_WindowHeatSw[0];
		js["ICE_WindowHeatSw_1"] = data.ICE_WindowHeatSw[0];
	}
	if (ngxData.ICE_WindowHeatSw[1] != data.ICE_WindowHeatSw[1]) {
		ngxData.ICE_WindowHeatSw[1] = data.ICE_WindowHeatSw[1];
		js["ICE_WindowHeatSw_2"] = data.ICE_WindowHeatSw[1];
	}
	if (ngxData.ICE_WindowHeatSw[2] != data.ICE_WindowHeatSw[2]) {
		ngxData.ICE_WindowHeatSw[2] = data.ICE_WindowHeatSw[2];
		js["ICE_WindowHeatSw_3"] = data.ICE_WindowHeatSw[2];
	}
	if (ngxData.ICE_WindowHeatSw[3] != data.ICE_WindowHeatSw[3]) {
		ngxData.ICE_WindowHeatSw[3] = data.ICE_WindowHeatSw[3];
		js["ICE_WindowHeatSw_4"] = data.ICE_WindowHeatSw[3];
	}

	if (ngxData.ICE_ProbeHeatSw[0] != data.ICE_ProbeHeatSw[0]) {
		ngxData.ICE_ProbeHeatSw[0] = data.ICE_ProbeHeatSw[0];
		js["ICE_ProbeHeatSw_1"] = data.ICE_ProbeHeatSw[0];
	}
	if (ngxData.ICE_ProbeHeatSw[1] != data.ICE_ProbeHeatSw[1]) {
		ngxData.ICE_ProbeHeatSw[1] = data.ICE_ProbeHeatSw[1];
		js["ICE_ProbeHeatSw_2"] = data.ICE_ProbeHeatSw[1];
	}
	if (ngxData.ICE_EngAntiIceSw[0] != data.ICE_EngAntiIceSw[0]) {
		ngxData.ICE_EngAntiIceSw[0] = data.ICE_EngAntiIceSw[0];
		js["ICE_EngAntiIceSw_1"] = data.ICE_EngAntiIceSw[0];
	}
	if (ngxData.ICE_EngAntiIceSw[1] != data.ICE_EngAntiIceSw[1]) {
		ngxData.ICE_EngAntiIceSw[1] = data.ICE_EngAntiIceSw[1];
		js["ICE_EngAntiIceSw_2"] = data.ICE_EngAntiIceSw[1];
	}
	if (ngxData.HYD_PumpSw_eng[0] != data.HYD_PumpSw_eng[0]) {
		ngxData.HYD_PumpSw_eng[0] = data.HYD_PumpSw_eng[0];
		js["HYD_PumpSw_eng_1"] = data.HYD_PumpSw_eng[0];
	}
	if (ngxData.HYD_PumpSw_eng[1] != data.HYD_PumpSw_eng[1]) {
		ngxData.HYD_PumpSw_eng[1] = data.HYD_PumpSw_eng[1];
		js["HYD_PumpSw_eng_2"] = data.HYD_PumpSw_eng[1];
	}
	if (ngxData.HYD_PumpSw_elec[0] != data.HYD_PumpSw_elec[0]) {
		ngxData.HYD_PumpSw_elec[0] = data.HYD_PumpSw_elec[0];
		js["HYD_PumpSw_elec_1"] = data.HYD_PumpSw_elec[0];
	}
	if (ngxData.HYD_PumpSw_elec[1] != data.HYD_PumpSw_elec[1]) {
		ngxData.HYD_PumpSw_elec[1] = data.HYD_PumpSw_elec[1];
		js["HYD_PumpSw_elec_2"] = data.HYD_PumpSw_elec[1];
	}
	if (ngxData.AIR_RecircFanSwitch[0] != data.AIR_RecircFanSwitch[0]) {
		ngxData.AIR_RecircFanSwitch[0] = data.AIR_RecircFanSwitch[0];
		js["AIR_RecircFanSwitch_1"] = data.AIR_RecircFanSwitch[0];
	}
	if (ngxData.AIR_RecircFanSwitch[1] != data.AIR_RecircFanSwitch[1]) {
		ngxData.AIR_RecircFanSwitch[1] = data.AIR_RecircFanSwitch[1];
		js["AIR_RecircFanSwitch_2"] = data.AIR_RecircFanSwitch[1];
	}
	if (ngxData.AIR_PackSwitch[0] != data.AIR_PackSwitch[0]) {
		ngxData.AIR_PackSwitch[0] = data.AIR_PackSwitch[0];
		js["AIR_PackSwitch_1"] = data.AIR_PackSwitch[0];
	}
	if (ngxData.AIR_PackSwitch[1] != data.AIR_PackSwitch[1]) {
		ngxData.AIR_PackSwitch[1] = data.AIR_PackSwitch[1];
		js["AIR_PackSwitch_2"] = data.AIR_PackSwitch[1];
	}
	if (ngxData.AIR_BleedAirSwitch[0] != data.AIR_BleedAirSwitch[0]) {
		ngxData.AIR_BleedAirSwitch[0] = data.AIR_BleedAirSwitch[0];
		js["AIR_BleedAirSwitch_1"] = data.AIR_BleedAirSwitch[0];
	}
	if (ngxData.AIR_BleedAirSwitch[1] != data.AIR_BleedAirSwitch[1]) {
		ngxData.AIR_BleedAirSwitch[1] = data.AIR_BleedAirSwitch[1];
		js["AIR_BleedAirSwitch_2"] = data.AIR_BleedAirSwitch[1];
	}
	if (ngxData.LTS_LandingLtRetractableSw[0] != data.LTS_LandingLtRetractableSw[0]) {
		ngxData.LTS_LandingLtRetractableSw[0] = data.LTS_LandingLtRetractableSw[0];
		js["LTS_LandingLtRetractableSw_1"] = data.LTS_LandingLtRetractableSw[0];
	}
	if (ngxData.LTS_LandingLtRetractableSw[1] != data.LTS_LandingLtRetractableSw[1]) {
		ngxData.LTS_LandingLtRetractableSw[1] = data.LTS_LandingLtRetractableSw[1];
		js["LTS_LandingLtRetractableSw_2"] = data.LTS_LandingLtRetractableSw[1];
	}
	if (ngxData.LTS_LandingLtFixedSw[0] != data.LTS_LandingLtFixedSw[0]) {
		ngxData.LTS_LandingLtFixedSw[0] = data.LTS_LandingLtFixedSw[0];
		js["LTS_LandingLtFixedSw_1"] = data.LTS_LandingLtFixedSw[0];
	}
	if (ngxData.LTS_LandingLtFixedSw[1] != data.LTS_LandingLtFixedSw[1]) {
		ngxData.LTS_LandingLtFixedSw[1] = data.LTS_LandingLtFixedSw[1];
		js["LTS_LandingLtFixedSw_2"] = data.LTS_LandingLtFixedSw[1];
	}
	if (ngxData.LTS_RunwayTurnoffSw[0] != data.LTS_RunwayTurnoffSw[0]) {
		ngxData.LTS_RunwayTurnoffSw[0] = data.LTS_RunwayTurnoffSw[0];
		js["LTS_RunwayTurnoffSw_1"] = data.LTS_RunwayTurnoffSw[0];
	}
	if (ngxData.LTS_RunwayTurnoffSw[1] != data.LTS_RunwayTurnoffSw[1]) {
		ngxData.LTS_RunwayTurnoffSw[1] = data.LTS_RunwayTurnoffSw[1];
		js["LTS_RunwayTurnoffSw_2"] = data.LTS_RunwayTurnoffSw[1];
	}
	if (ngxData.ENG_StartSelector[0] != data.ENG_StartSelector[0]) {
		ngxData.ENG_StartSelector[0] = data.ENG_StartSelector[0];
		js["ENG_StartSelector_1"] = data.ENG_StartSelector[0];
	}
	if (ngxData.ENG_StartSelector[1] != data.ENG_StartSelector[1]) {
		ngxData.ENG_StartSelector[1] = data.ENG_StartSelector[1];
		js["ENG_StartSelector_2"] = data.ENG_StartSelector[1];
	}
	//if (ngxData.EFIS_MinsSelBARO[0] != data.EFIS_MinsSelBARO[0]) {
	//	ngxData.EFIS_MinsSelBARO[0] = data.EFIS_MinsSelBARO[0];
	//	js["EFIS_MinsSelBARO_1"] = data.EFIS_MinsSelBARO[0];
	//}
	//if (ngxData.EFIS_MinsSelBARO[1] != data.EFIS_MinsSelBARO[1]) {
	//	ngxData.EFIS_MinsSelBARO[1] = data.EFIS_MinsSelBARO[1];
	//	js["EFIS_MinsSelBARO_2"] = data.EFIS_MinsSelBARO[1];
	//}
	//if (ngxData.EFIS_BaroSelHPA[0] != data.EFIS_BaroSelHPA[0]) {
	//	ngxData.EFIS_BaroSelHPA[0] = data.EFIS_BaroSelHPA[0];
	//	js["EFIS_BaroSelHPA_1"] = data.EFIS_BaroSelHPA[0];
	//}
	//if (ngxData.EFIS_BaroSelHPA[1] != data.EFIS_BaroSelHPA[1]) {
	//	ngxData.EFIS_BaroSelHPA[1] = data.EFIS_BaroSelHPA[1];
	//	js["EFIS_BaroSelHPA_2"] = data.EFIS_BaroSelHPA[1];
	//}
	if (ngxData.MAIN_MainPanelDUSel[0] != data.MAIN_MainPanelDUSel[0]) {
		ngxData.MAIN_MainPanelDUSel[0] = data.MAIN_MainPanelDUSel[0];
		js["MAIN_MainPanelDUSel_1"] = data.MAIN_MainPanelDUSel[0];
	}
	if (ngxData.MAIN_MainPanelDUSel[1] != data.MAIN_MainPanelDUSel[1]) {
		ngxData.MAIN_MainPanelDUSel[1] = data.MAIN_MainPanelDUSel[1];
		js["MAIN_MainPanelDUSel_2"] = data.MAIN_MainPanelDUSel[1];
	}
	if (ngxData.MAIN_LowerDUSel[0] != data.MAIN_LowerDUSel[0]) {
		ngxData.MAIN_LowerDUSel[0] = data.MAIN_LowerDUSel[0];
		js["MAIN_LowerDUSel_1"] = data.MAIN_LowerDUSel[0];
	}
	if (ngxData.MAIN_LowerDUSel[1] != data.MAIN_LowerDUSel[1]) {
		ngxData.MAIN_LowerDUSel[1] = data.MAIN_LowerDUSel[1];
		js["MAIN_LowerDUSel_2"] = data.MAIN_LowerDUSel[1];
	}
	//if (ngxData.MAIN_DisengageTestSelector[0] != data.MAIN_DisengageTestSelector[0]) {
	//	ngxData.MAIN_DisengageTestSelector[0] = data.MAIN_DisengageTestSelector[0];
	//	js["MAIN_DisengageTestSelector_1"] = data.MAIN_DisengageTestSelector[0];
	//}
	//if (ngxData.MAIN_DisengageTestSelector[1] != data.MAIN_DisengageTestSelector[1]) {
	//	ngxData.MAIN_DisengageTestSelector[1] = data.MAIN_DisengageTestSelector[1];
	//	js["MAIN_DisengageTestSelector_2"] = data.MAIN_DisengageTestSelector[1];
	//}
	if (ngxData.FIRE_OvhtDetSw[0] != data.FIRE_OvhtDetSw[0]) {
		ngxData.FIRE_OvhtDetSw[0] = data.FIRE_OvhtDetSw[0];
		js["FIRE_OvhtDetSw_1"] = data.FIRE_OvhtDetSw[0];
	}
	if (ngxData.FIRE_OvhtDetSw[1] != data.FIRE_OvhtDetSw[1]) {
		ngxData.FIRE_OvhtDetSw[1] = data.FIRE_OvhtDetSw[1];
		js["FIRE_OvhtDetSw_2"] = data.FIRE_OvhtDetSw[1];
	}
	//if (ngxData.FIRE_HandlePos[0] != data.FIRE_HandlePos[0]) {
	//	ngxData.FIRE_HandlePos[0] = data.FIRE_HandlePos[0];
	//	js["FIRE_HandlePos_1"] = data.FIRE_HandlePos[0];
	//}
	//if (ngxData.FIRE_HandlePos[1] != data.FIRE_HandlePos[1]) {
	//	ngxData.FIRE_HandlePos[1] = data.FIRE_HandlePos[1];
	//	js["FIRE_HandlePos_2"] = data.FIRE_HandlePos[1];
	//}
	//if (ngxData.FIRE_HandlePos[2] != data.FIRE_HandlePos[2]) {
	//	ngxData.FIRE_HandlePos[2] = data.FIRE_HandlePos[2];
	//	js["FIRE_HandlePos_3"] = data.FIRE_HandlePos[2];
	//}
	//if (ngxData.FIRE_HandleIlluminated[0] != data.FIRE_HandleIlluminated[0]) {
	//	ngxData.FIRE_HandleIlluminated[0] = data.FIRE_HandleIlluminated[0];
	//	js["FIRE_HandleIlluminated_1"] = data.FIRE_HandleIlluminated[0];
	//}
	//if (ngxData.FIRE_HandleIlluminated[1] != data.FIRE_HandleIlluminated[1]) {
	//	ngxData.FIRE_HandleIlluminated[1] = data.FIRE_HandleIlluminated[1];
	//	js["FIRE_HandleIlluminated_2"] = data.FIRE_HandleIlluminated[1];
	//}
	//if (ngxData.FIRE_HandleIlluminated[2] != data.FIRE_HandleIlluminated[2]) {
	//	ngxData.FIRE_HandleIlluminated[2] = data.FIRE_HandleIlluminated[2];
	//	js["FIRE_HandleIlluminated_3"] = data.FIRE_HandleIlluminated[2];
	//}
	if (ngxData.CARGO_DetSelect[0] != data.CARGO_DetSelect[0]) {
		ngxData.CARGO_DetSelect[0] = data.CARGO_DetSelect[0];
		js["CARGO_DetSelect_1"] = data.CARGO_DetSelect[0];
	}
	if (ngxData.CARGO_DetSelect[1] != data.CARGO_DetSelect[1]) {
		ngxData.CARGO_DetSelect[1] = data.CARGO_DetSelect[1];
		js["CARGO_DetSelect_2"] = data.CARGO_DetSelect[1];
	}
	//if (ngxData.CARGO_ArmedSw[0] != data.CARGO_ArmedSw[0]) {
	//	ngxData.CARGO_ArmedSw[0] = data.CARGO_ArmedSw[0];
	//	js["CARGO_ArmedSw_1"] = data.CARGO_ArmedSw[0];
	//}
	//if (ngxData.CARGO_ArmedSw[1] != data.CARGO_ArmedSw[1]) {
	//	ngxData.CARGO_ArmedSw[1] = data.CARGO_ArmedSw[1];
	//	js["CARGO_ArmedSw_2"] = data.CARGO_ArmedSw[1];
	//}
	if (ngxData.ENG_StartValve[0] != data.ENG_StartValve[0]) {
		ngxData.ENG_StartValve[0] = data.ENG_StartValve[0];
		js["ENG_StartValve_1"] = data.ENG_StartValve[0];
	}
	if (ngxData.ENG_StartValve[1] != data.ENG_StartValve[1]) {
		ngxData.ENG_StartValve[1] = data.ENG_StartValve[1];
		js["ENG_StartValve_2"] = data.ENG_StartValve[1];
	}
	//if (ngxData.COMM_SelectedMic[0] != data.COMM_SelectedMic[0]) {
	//	ngxData.COMM_SelectedMic[0] = data.COMM_SelectedMic[0];
	//	js["COMM_SelectedMic_1"] = data.COMM_SelectedMic[0];
	//}
	//if (ngxData.COMM_SelectedMic[1] != data.COMM_SelectedMic[1]) {
	//	ngxData.COMM_SelectedMic[1] = data.COMM_SelectedMic[1];
	//	js["COMM_SelectedMic_2"] = data.COMM_SelectedMic[1];
	//}
	//if (ngxData.COMM_SelectedMic[2] != data.COMM_SelectedMic[2]) {
	//	ngxData.COMM_SelectedMic[2] = data.COMM_SelectedMic[2];
	//	js["COMM_SelectedMic_3"] = data.COMM_SelectedMic[2];
	//}
	//if (ngxData.COMM_ReceiverSwitches[0] != data.COMM_ReceiverSwitches[0]) {
	//	ngxData.COMM_ReceiverSwitches[0] = data.COMM_ReceiverSwitches[0];
	//	js["COMM_ReceiverSwitches_1"] = data.COMM_ReceiverSwitches[0];
	//}
	//if (ngxData.COMM_ReceiverSwitches[1] != data.COMM_ReceiverSwitches[1]) {
	//	ngxData.COMM_ReceiverSwitches[1] = data.COMM_ReceiverSwitches[1];
	//	js["COMM_ReceiverSwitches_2"] = data.COMM_ReceiverSwitches[1];
	//}
	//if (ngxData.COMM_ReceiverSwitches[2] != data.COMM_ReceiverSwitches[2]) {
	//	ngxData.COMM_ReceiverSwitches[2] = data.COMM_ReceiverSwitches[2];
	//	js["COMM_ReceiverSwitches_3"] = data.COMM_ReceiverSwitches[2];
	//}


	if (ngxData.IRS_annunGPS != data.IRS_annunGPS) {
		ngxData.IRS_annunGPS = data.IRS_annunGPS;
		js["IRS_annunGPS"] = data.IRS_annunGPS;
	}
	if (ngxData.WARN_annunPSEU != data.WARN_annunPSEU) {
		ngxData.WARN_annunPSEU = data.WARN_annunPSEU;
		js["WARN_annunPSEU"] = data.WARN_annunPSEU;
	}
	if (ngxData.OXY_annunPASS_OXY_ON != data.OXY_annunPASS_OXY_ON) {
		ngxData.OXY_annunPASS_OXY_ON = data.OXY_annunPASS_OXY_ON;
		js["OXY_annunPASS_OXY_ON"] = data.OXY_annunPASS_OXY_ON;
	}
	if (ngxData.GEAR_annunOvhdLEFT != data.GEAR_annunOvhdLEFT) {
		ngxData.GEAR_annunOvhdLEFT = data.GEAR_annunOvhdLEFT;
		js["GEAR_annunOvhdLEFT"] = data.GEAR_annunOvhdLEFT;
	}
	if (ngxData.GEAR_annunOvhdNOSE != data.GEAR_annunOvhdNOSE) {
		ngxData.GEAR_annunOvhdNOSE = data.GEAR_annunOvhdNOSE;
		js["GEAR_annunOvhdNOSE"] = data.GEAR_annunOvhdNOSE;
	}
	if (ngxData.GEAR_annunOvhdRIGHT != data.GEAR_annunOvhdRIGHT) {
		ngxData.GEAR_annunOvhdRIGHT = data.GEAR_annunOvhdRIGHT;
		js["GEAR_annunOvhdRIGHT"] = data.GEAR_annunOvhdRIGHT;
	}
	//if (ngxData.FLTREC_annunOFF != data.FLTREC_annunOFF) {
	//	ngxData.FLTREC_annunOFF = data.FLTREC_annunOFF;
	//	js["FLTREC_annunOFF"] = data.FLTREC_annunOFF;
	//}
	if (ngxData.FCTL_annunYAW_DAMPER != data.FCTL_annunYAW_DAMPER) {
		ngxData.FCTL_annunYAW_DAMPER = data.FCTL_annunYAW_DAMPER;
		js["FCTL_annunYAW_DAMPER"] = data.FCTL_annunYAW_DAMPER;
	}
	if (ngxData.FCTL_annunLOW_QUANTITY != data.FCTL_annunLOW_QUANTITY) {
		ngxData.FCTL_annunLOW_QUANTITY = data.FCTL_annunLOW_QUANTITY;
		js["FCTL_annunLOW_QUANTITY"] = data.FCTL_annunLOW_QUANTITY;
	}
	if (ngxData.FCTL_annunLOW_PRESSURE != data.FCTL_annunLOW_PRESSURE) {
		ngxData.FCTL_annunLOW_PRESSURE = data.FCTL_annunLOW_PRESSURE;
		js["FCTL_annunLOW_PRESSURE"] = data.FCTL_annunLOW_PRESSURE;
	}
	if (ngxData.FCTL_annunLOW_STBY_RUD_ON != data.FCTL_annunLOW_STBY_RUD_ON) {
		ngxData.FCTL_annunLOW_STBY_RUD_ON = data.FCTL_annunLOW_STBY_RUD_ON;
		js["FCTL_annunLOW_STBY_RUD_ON"] = data.FCTL_annunLOW_STBY_RUD_ON;
	}
	if (ngxData.FCTL_annunFEEL_DIFF_PRESS != data.FCTL_annunFEEL_DIFF_PRESS) {
		ngxData.FCTL_annunFEEL_DIFF_PRESS = data.FCTL_annunFEEL_DIFF_PRESS;
		js["FCTL_annunFEEL_DIFF_PRESS"] = data.FCTL_annunFEEL_DIFF_PRESS;
	}
	if (ngxData.FCTL_annunSPEED_TRIM_FAIL != data.FCTL_annunSPEED_TRIM_FAIL) {
		ngxData.FCTL_annunSPEED_TRIM_FAIL = data.FCTL_annunSPEED_TRIM_FAIL;
		js["FCTL_annunSPEED_TRIM_FAIL"] = data.FCTL_annunSPEED_TRIM_FAIL;
	}
	if (ngxData.FCTL_annunMACH_TRIM_FAIL != data.FCTL_annunMACH_TRIM_FAIL) {
		ngxData.FCTL_annunMACH_TRIM_FAIL = data.FCTL_annunMACH_TRIM_FAIL;
		js["FCTL_annunMACH_TRIM_FAIL"] = data.FCTL_annunMACH_TRIM_FAIL;
	}
	if (ngxData.FCTL_annunAUTO_SLAT_FAIL != data.FCTL_annunAUTO_SLAT_FAIL) {
		ngxData.FCTL_annunAUTO_SLAT_FAIL = data.FCTL_annunAUTO_SLAT_FAIL;
		js["FCTL_annunAUTO_SLAT_FAIL"] = data.FCTL_annunAUTO_SLAT_FAIL;
	}
	if (ngxData.FUEL_annunXFEED_VALVE_OPEN != data.FUEL_annunXFEED_VALVE_OPEN) {
		ngxData.FUEL_annunXFEED_VALVE_OPEN = data.FUEL_annunXFEED_VALVE_OPEN;
		js["FUEL_annunXFEED_VALVE_OPEN"] = data.FUEL_annunXFEED_VALVE_OPEN;
	}
	if (ngxData.ELEC_annunBAT_DISCHARGE != data.ELEC_annunBAT_DISCHARGE) {
		ngxData.ELEC_annunBAT_DISCHARGE = data.ELEC_annunBAT_DISCHARGE;
		js["ELEC_annunBAT_DISCHARGE"] = data.ELEC_annunBAT_DISCHARGE;
	}
	if (ngxData.ELEC_annunTR_UNIT != data.ELEC_annunTR_UNIT) {
		ngxData.ELEC_annunTR_UNIT = data.ELEC_annunTR_UNIT;
		js["ELEC_annunTR_UNIT"] = data.ELEC_annunTR_UNIT;
	}
	if (ngxData.ELEC_annunELEC != data.ELEC_annunELEC) {
		ngxData.ELEC_annunELEC = data.ELEC_annunELEC;
		js["ELEC_annunELEC"] = data.ELEC_annunELEC;
	}
	if (ngxData.ELEC_annunSTANDBY_POWER_OFF != data.ELEC_annunSTANDBY_POWER_OFF) {
		ngxData.ELEC_annunSTANDBY_POWER_OFF = data.ELEC_annunSTANDBY_POWER_OFF;
		js["ELEC_annunSTANDBY_POWER_OFF"] = data.ELEC_annunSTANDBY_POWER_OFF;
	}
	if (ngxData.ELEC_annunGRD_POWER_AVAILABLE != data.ELEC_annunGRD_POWER_AVAILABLE) {
		ngxData.ELEC_annunGRD_POWER_AVAILABLE = data.ELEC_annunGRD_POWER_AVAILABLE;
		js["ELEC_annunGRD_POWER_AVAILABLE"] = data.ELEC_annunGRD_POWER_AVAILABLE;
	}
	if (ngxData.ELEC_annunAPU_GEN_OFF_BUS != data.ELEC_annunAPU_GEN_OFF_BUS) {
		ngxData.ELEC_annunAPU_GEN_OFF_BUS = data.ELEC_annunAPU_GEN_OFF_BUS;
		js["ELEC_annunAPU_GEN_OFF_BUS"] = data.ELEC_annunAPU_GEN_OFF_BUS;
	}
	//if (ngxData.APU_annunMAINT != data.APU_annunMAINT) {
	//	ngxData.APU_annunMAINT = data.APU_annunMAINT;
	//	js["APU_annunMAINT"] = data.APU_annunMAINT;
	//}
	//if (ngxData.APU_annunLOW_OIL_PRESSURE != data.APU_annunLOW_OIL_PRESSURE) {
	//	ngxData.APU_annunLOW_OIL_PRESSURE = data.APU_annunLOW_OIL_PRESSURE;
	//	js["APU_annunLOW_OIL_PRESSURE"] = data.APU_annunLOW_OIL_PRESSURE;
	//}
	//if (ngxData.APU_annunFAULT != data.APU_annunFAULT) {
	//	ngxData.APU_annunFAULT = data.APU_annunFAULT;
	//	js["APU_annunFAULT"] = data.APU_annunFAULT;
	//}
	//if (ngxData.APU_annunOVERSPEED != data.APU_annunOVERSPEED) {
	//	ngxData.APU_annunOVERSPEED = data.APU_annunOVERSPEED;
	//	js["APU_annunOVERSPEED"] = data.APU_annunOVERSPEED;
	//}
	if (ngxData.AIR_annunEquipCoolingSupplyOFF != data.AIR_annunEquipCoolingSupplyOFF) {
		ngxData.AIR_annunEquipCoolingSupplyOFF = data.AIR_annunEquipCoolingSupplyOFF;
		js["AIR_annunEquipCoolingSupplyOFF"] = data.AIR_annunEquipCoolingSupplyOFF;
	}
	if (ngxData.AIR_annunEquipCoolingExhaustOFF != data.AIR_annunEquipCoolingExhaustOFF) {
		ngxData.AIR_annunEquipCoolingExhaustOFF = data.AIR_annunEquipCoolingExhaustOFF;
		js["AIR_annunEquipCoolingExhaustOFF"] = data.AIR_annunEquipCoolingExhaustOFF;
	}
	if (ngxData.LTS_annunEmerNOT_ARMED != data.LTS_annunEmerNOT_ARMED) {
		ngxData.LTS_annunEmerNOT_ARMED = data.LTS_annunEmerNOT_ARMED;
		js["LTS_annunEmerNOT_ARMED"] = data.LTS_annunEmerNOT_ARMED;
	}
	//if (ngxData.COMM_annunCALL != data.COMM_annunCALL) {
	//	ngxData.COMM_annunCALL = data.COMM_annunCALL;
	//	js["COMM_annunCALL"] = data.COMM_annunCALL;
	//}
	//if (ngxData.COMM_annunPA_IN_USE != data.COMM_annunPA_IN_USE) {
	//	ngxData.COMM_annunPA_IN_USE = data.COMM_annunPA_IN_USE;
	//	js["COMM_annunPA_IN_USE"] = data.COMM_annunPA_IN_USE;
	//}
	if (ngxData.ICE_annunCAPT_PITOT != data.ICE_annunCAPT_PITOT) {
		ngxData.ICE_annunCAPT_PITOT = data.ICE_annunCAPT_PITOT;
		js["ICE_annunCAPT_PITOT"] = data.ICE_annunCAPT_PITOT;
	}
	if (ngxData.ICE_annunL_ELEV_PITOT != data.ICE_annunL_ELEV_PITOT) {
		ngxData.ICE_annunL_ELEV_PITOT = data.ICE_annunL_ELEV_PITOT;
		js["ICE_annunL_ELEV_PITOT"] = data.ICE_annunL_ELEV_PITOT;
	}
	if (ngxData.ICE_annunL_ALPHA_VANE != data.ICE_annunL_ALPHA_VANE) {
		ngxData.ICE_annunL_ALPHA_VANE = data.ICE_annunL_ALPHA_VANE;
		js["ICE_annunL_ALPHA_VANE"] = data.ICE_annunL_ALPHA_VANE;
	}
	if (ngxData.ICE_annunL_TEMP_PROBE != data.ICE_annunL_TEMP_PROBE) {
		ngxData.ICE_annunL_TEMP_PROBE = data.ICE_annunL_TEMP_PROBE;
		js["ICE_annunL_TEMP_PROBE"] = data.ICE_annunL_TEMP_PROBE;
	}
	if (ngxData.ICE_annunFO_PITOT != data.ICE_annunFO_PITOT) {
		ngxData.ICE_annunFO_PITOT = data.ICE_annunFO_PITOT;
		js["ICE_annunFO_PITOT"] = data.ICE_annunFO_PITOT;
	}
	if (ngxData.ICE_annunR_ELEV_PITOT != data.ICE_annunR_ELEV_PITOT) {
		ngxData.ICE_annunR_ELEV_PITOT = data.ICE_annunR_ELEV_PITOT;
		js["ICE_annunR_ELEV_PITOT"] = data.ICE_annunR_ELEV_PITOT;
	}
	if (ngxData.ICE_annunR_ALPHA_VANE != data.ICE_annunR_ALPHA_VANE) {
		ngxData.ICE_annunR_ALPHA_VANE = data.ICE_annunR_ALPHA_VANE;
		js["ICE_annunR_ALPHA_VANE"] = data.ICE_annunR_ALPHA_VANE;
	}
	if (ngxData.ICE_annunAUX_PITOT != data.ICE_annunAUX_PITOT) {
		ngxData.ICE_annunAUX_PITOT = data.ICE_annunAUX_PITOT;
		js["ICE_annunAUX_PITOT"] = data.ICE_annunAUX_PITOT;
	}
	if (ngxData.AIR_annunDualBleed != data.AIR_annunDualBleed) {
		ngxData.AIR_annunDualBleed = data.AIR_annunDualBleed;
		js["AIR_annunDualBleed"] = data.AIR_annunDualBleed;
	}
	if (ngxData.AIR_annunRamDoorL != data.AIR_annunRamDoorL) {
		ngxData.AIR_annunRamDoorL = data.AIR_annunRamDoorL;
		js["AIR_annunRamDoorL"] = data.AIR_annunRamDoorL;
	}
	if (ngxData.AIR_annunRamDoorR != data.AIR_annunRamDoorR) {
		ngxData.AIR_annunRamDoorR = data.AIR_annunRamDoorR;
		js["AIR_annunRamDoorR"] = data.AIR_annunRamDoorR;
	}
	//if (ngxData.AIR_FltAltWindow != data.AIR_FltAltWindow) {
	//	ngxData.AIR_FltAltWindow = data.AIR_FltAltWindow;
	//	js["AIR_FltAltWindow"] = data.AIR_FltAltWindow;
	//}
	//if (ngxData.AIR_LandAltWindow != data.AIR_LandAltWindow) {
	//	ngxData.AIR_LandAltWindow = data.AIR_LandAltWindow;
	//	js["AIR_LandAltWindow"] = data.AIR_LandAltWindow;
	//}
	if (ngxData.WARN_annunFLT_CONT != data.WARN_annunFLT_CONT) {
		ngxData.WARN_annunFLT_CONT = data.WARN_annunFLT_CONT;
		js["WARN_annunFLT_CONT"] = data.WARN_annunFLT_CONT;
	}
	if (ngxData.WARN_annunIRS != data.WARN_annunIRS) {
		ngxData.WARN_annunIRS = data.WARN_annunIRS;
		js["WARN_annunIRS"] = data.WARN_annunIRS;
	}
	if (ngxData.WARN_annunFUEL != data.WARN_annunFUEL) {
		ngxData.WARN_annunFUEL = data.WARN_annunFUEL;
		js["WARN_annunFUEL"] = data.WARN_annunFUEL;
	}
	if (ngxData.WARN_annunELEC != data.WARN_annunELEC) {
		ngxData.WARN_annunELEC = data.WARN_annunELEC;
		js["WARN_annunELEC"] = data.WARN_annunELEC;
	}
	if (ngxData.WARN_annunAPU != data.WARN_annunAPU) {
		ngxData.WARN_annunAPU = data.WARN_annunAPU;
		js["WARN_annunAPU"] = data.WARN_annunAPU;
	}
	if (ngxData.WARN_annunOVHT_DET != data.WARN_annunOVHT_DET) {
		ngxData.WARN_annunOVHT_DET = data.WARN_annunOVHT_DET;
		js["WARN_annunOVHT_DET"] = data.WARN_annunOVHT_DET;
	}
	if (ngxData.WARN_annunANTI_ICE != data.WARN_annunANTI_ICE) {
		ngxData.WARN_annunANTI_ICE = data.WARN_annunANTI_ICE;
		js["WARN_annunANTI_ICE"] = data.WARN_annunANTI_ICE;
	}
	if (ngxData.WARN_annunHYD != data.WARN_annunHYD) {
		ngxData.WARN_annunHYD = data.WARN_annunHYD;
		js["WARN_annunHYD"] = data.WARN_annunHYD;
	}
	if (ngxData.WARN_annunDOORS != data.WARN_annunDOORS) {
		ngxData.WARN_annunDOORS = data.WARN_annunDOORS;
		js["WARN_annunDOORS"] = data.WARN_annunDOORS;
	}
	if (ngxData.WARN_annunENG != data.WARN_annunENG) {
		ngxData.WARN_annunENG = data.WARN_annunENG;
		js["WARN_annunENG"] = data.WARN_annunENG;
	}
	if (ngxData.WARN_annunOVERHEAD != data.WARN_annunOVERHEAD) {
		ngxData.WARN_annunOVERHEAD = data.WARN_annunOVERHEAD;
		js["WARN_annunOVERHEAD"] = data.WARN_annunOVERHEAD;
	}
	if (ngxData.WARN_annunAIR_COND != data.WARN_annunAIR_COND) {
		ngxData.WARN_annunAIR_COND = data.WARN_annunAIR_COND;
		js["WARN_annunAIR_COND"] = data.WARN_annunAIR_COND;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_ARMED != data.MAIN_annunSPEEDBRAKE_ARMED) {
		ngxData.MAIN_annunSPEEDBRAKE_ARMED = data.MAIN_annunSPEEDBRAKE_ARMED;
		js["MAIN_annunSPEEDBRAKE_ARMED"] = data.MAIN_annunSPEEDBRAKE_ARMED;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM != data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM) {
		ngxData.MAIN_annunSPEEDBRAKE_DO_NOT_ARM = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM;
		js["MAIN_annunSPEEDBRAKE_DO_NOT_ARM"] = data.MAIN_annunSPEEDBRAKE_DO_NOT_ARM;
	}
	if (ngxData.MAIN_annunSPEEDBRAKE_EXTENDED != data.MAIN_annunSPEEDBRAKE_EXTENDED) {
		ngxData.MAIN_annunSPEEDBRAKE_EXTENDED = data.MAIN_annunSPEEDBRAKE_EXTENDED;
		js["MAIN_annunSPEEDBRAKE_EXTENDED"] = data.MAIN_annunSPEEDBRAKE_EXTENDED;
	}
	if (ngxData.MAIN_annunSTAB_OUT_OF_TRIM != data.MAIN_annunSTAB_OUT_OF_TRIM) {
		ngxData.MAIN_annunSTAB_OUT_OF_TRIM = data.MAIN_annunSTAB_OUT_OF_TRIM;
		js["MAIN_annunSTAB_OUT_OF_TRIM"] = data.MAIN_annunSTAB_OUT_OF_TRIM;
	}
	if (ngxData.MAIN_annunANTI_SKID_INOP != data.MAIN_annunANTI_SKID_INOP) {
		ngxData.MAIN_annunANTI_SKID_INOP = data.MAIN_annunANTI_SKID_INOP;
		js["MAIN_annunANTI_SKID_INOP"] = data.MAIN_annunANTI_SKID_INOP;
	}
	if (ngxData.MAIN_annunAUTO_BRAKE_DISARM != data.MAIN_annunAUTO_BRAKE_DISARM) {
		ngxData.MAIN_annunAUTO_BRAKE_DISARM = data.MAIN_annunAUTO_BRAKE_DISARM;
		js["MAIN_annunAUTO_BRAKE_DISARM"] = data.MAIN_annunAUTO_BRAKE_DISARM;
	}
	//if (ngxData.MAIN_annunLE_FLAPS_TRANSIT != data.MAIN_annunLE_FLAPS_TRANSIT) {
	//	ngxData.MAIN_annunLE_FLAPS_TRANSIT = data.MAIN_annunLE_FLAPS_TRANSIT;
	//	js["MAIN_annunLE_FLAPS_TRANSIT"] = data.MAIN_annunLE_FLAPS_TRANSIT;
	//}
	//if (ngxData.MAIN_annunLE_FLAPS_EXT != data.MAIN_annunLE_FLAPS_EXT) {
	//	ngxData.MAIN_annunLE_FLAPS_EXT = data.MAIN_annunLE_FLAPS_EXT;
	//	js["MAIN_annunLE_FLAPS_EXT"] = data.MAIN_annunLE_FLAPS_EXT;
	//}
	//if (ngxData.HGS_annun_AIII != data.HGS_annun_AIII) {
	//	ngxData.HGS_annun_AIII = data.HGS_annun_AIII;
	//	js["HGS_annun_AIII"] = data.HGS_annun_AIII;
	//}
	//if (ngxData.HGS_annun_NO_AIII != data.HGS_annun_NO_AIII) {
	//	ngxData.HGS_annun_NO_AIII = data.HGS_annun_NO_AIII;
	//	js["HGS_annun_NO_AIII"] = data.HGS_annun_NO_AIII;
	//}
	//if (ngxData.HGS_annun_FLARE != data.HGS_annun_FLARE) {
	//	ngxData.HGS_annun_FLARE = data.HGS_annun_FLARE;
	//	js["HGS_annun_FLARE"] = data.HGS_annun_FLARE;
	//}
	//if (ngxData.HGS_annun_RO != data.HGS_annun_RO) {
	//	ngxData.HGS_annun_RO = data.HGS_annun_RO;
	//	js["HGS_annun_RO"] = data.HGS_annun_RO;
	//}
	//if (ngxData.HGS_annun_RO_CTN != data.HGS_annun_RO_CTN) {
	//	ngxData.HGS_annun_RO_CTN = data.HGS_annun_RO_CTN;
	//	js["HGS_annun_RO_CTN"] = data.HGS_annun_RO_CTN;
	//}
	//if (ngxData.HGS_annun_RO_ARM != data.HGS_annun_RO_ARM) {
	//	ngxData.HGS_annun_RO_ARM = data.HGS_annun_RO_ARM;
	//	js["HGS_annun_RO_ARM"] = data.HGS_annun_RO_ARM;
	//}
	//if (ngxData.HGS_annun_TO != data.HGS_annun_TO) {
	//	ngxData.HGS_annun_TO = data.HGS_annun_TO;
	//	js["HGS_annun_TO"] = data.HGS_annun_TO;
	//}
	//if (ngxData.HGS_annun_TO_CTN != data.HGS_annun_TO_CTN) {
	//	ngxData.HGS_annun_TO_CTN = data.HGS_annun_TO_CTN;
	//	js["HGS_annun_TO_CTN"] = data.HGS_annun_TO_CTN;
	//}
	//if (ngxData.HGS_annun_APCH != data.HGS_annun_APCH) {
	//	ngxData.HGS_annun_APCH = data.HGS_annun_APCH;
	//	js["HGS_annun_APCH"] = data.HGS_annun_APCH;
	//}
	//if (ngxData.HGS_annun_TO_WARN != data.HGS_annun_TO_WARN) {
	//	ngxData.HGS_annun_TO_WARN = data.HGS_annun_TO_WARN;
	//	js["HGS_annun_TO_WARN"] = data.HGS_annun_TO_WARN;
	//}
	//if (ngxData.HGS_annun_Bar != data.HGS_annun_Bar) {
	//	ngxData.HGS_annun_Bar = data.HGS_annun_Bar;
	//	js["HGS_annun_Bar"] = data.HGS_annun_Bar;
	//}
	//if (ngxData.HGS_annun_FAIL != data.HGS_annun_FAIL) {
	//	ngxData.HGS_annun_FAIL = data.HGS_annun_FAIL;
	//	js["HGS_annun_FAIL"] = data.HGS_annun_FAIL;
	//}
	//if (ngxData.GPWS_annunINOP != data.GPWS_annunINOP) {
	//	ngxData.GPWS_annunINOP = data.GPWS_annunINOP;
	//	js["GPWS_annunINOP"] = data.GPWS_annunINOP;
	//}
	if (ngxData.PED_annunParkingBrake != data.PED_annunParkingBrake) {
		ngxData.PED_annunParkingBrake = data.PED_annunParkingBrake;
		js["PED_annunParkingBrake"] = data.PED_annunParkingBrake;
	}
	//if (ngxData.FIRE_annunWHEEL_WELL != data.FIRE_annunWHEEL_WELL) {
	//	ngxData.FIRE_annunWHEEL_WELL = data.FIRE_annunWHEEL_WELL;
	//	js["FIRE_annunWHEEL_WELL"] = data.FIRE_annunWHEEL_WELL;
	//}
	//if (ngxData.FIRE_annunFAULT != data.FIRE_annunFAULT) {
	//	ngxData.FIRE_annunFAULT = data.FIRE_annunFAULT;
	//	js["FIRE_annunFAULT"] = data.FIRE_annunFAULT;
	//}
	//if (ngxData.FIRE_annunAPU_DET_INOP != data.FIRE_annunAPU_DET_INOP) {
	//	ngxData.FIRE_annunAPU_DET_INOP = data.FIRE_annunAPU_DET_INOP;
	//	js["FIRE_annunAPU_DET_INOP"] = data.FIRE_annunAPU_DET_INOP;
	//}
	//if (ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE != data.FIRE_annunAPU_BOTTLE_DISCHARGE) {
	//	ngxData.FIRE_annunAPU_BOTTLE_DISCHARGE = data.FIRE_annunAPU_BOTTLE_DISCHARGE;
	//	js["FIRE_annunAPU_BOTTLE_DISCHARGE"] = data.FIRE_annunAPU_BOTTLE_DISCHARGE;
	//}
	//if (ngxData.CARGO_annunFWD != data.CARGO_annunFWD) {
	//	ngxData.CARGO_annunFWD = data.CARGO_annunFWD;
	//	js["CARGO_annunFWD"] = data.CARGO_annunFWD;
	//}
	//if (ngxData.CARGO_annunAFT != data.CARGO_annunAFT) {
	//	ngxData.CARGO_annunAFT = data.CARGO_annunAFT;
	//	js["CARGO_annunAFT"] = data.CARGO_annunAFT;
	//}
	//if (ngxData.CARGO_annunDETECTOR_FAULT != data.CARGO_annunDETECTOR_FAULT) {
	//	ngxData.CARGO_annunDETECTOR_FAULT = data.CARGO_annunDETECTOR_FAULT;
	//	js["CARGO_annunDETECTOR_FAULT"] = data.CARGO_annunDETECTOR_FAULT;
	//}
	//if (ngxData.CARGO_annunDISCH != data.CARGO_annunDISCH) {
	//	ngxData.CARGO_annunDISCH = data.CARGO_annunDISCH;
	//	js["CARGO_annunDISCH"] = data.CARGO_annunDISCH;
	//}
	//if (ngxData.HGS_annunRWY != data.HGS_annunRWY) {
	//	ngxData.HGS_annunRWY = data.HGS_annunRWY;
	//	js["HGS_annunRWY"] = data.HGS_annunRWY;
	//}
	//if (ngxData.HGS_annunGS != data.HGS_annunGS) {
	//	ngxData.HGS_annunGS = data.HGS_annunGS;
	//	js["HGS_annunGS"] = data.HGS_annunGS;
	//}
	//if (ngxData.HGS_annunFAULT != data.HGS_annunFAULT) {
	//	ngxData.HGS_annunFAULT = data.HGS_annunFAULT;
	//	js["HGS_annunFAULT"] = data.HGS_annunFAULT;
	//}
	//if (ngxData.HGS_annunCLR != data.HGS_annunCLR) {
	//	ngxData.HGS_annunCLR = data.HGS_annunCLR;
	//	js["HGS_annunCLR"] = data.HGS_annunCLR;
	//}
	//if (ngxData.XPDR_annunFAIL != data.XPDR_annunFAIL) {
	//	ngxData.XPDR_annunFAIL = data.XPDR_annunFAIL;
	//	js["XPDR_annunFAIL"] = data.XPDR_annunFAIL;
	//}
	//if (ngxData.PED_annunLOCK_FAIL != data.PED_annunLOCK_FAIL) {
	//	ngxData.PED_annunLOCK_FAIL = data.PED_annunLOCK_FAIL;
	//	js["PED_annunLOCK_FAIL"] = data.PED_annunLOCK_FAIL;
	//}
	//if (ngxData.PED_annunAUTO_UNLK != data.PED_annunAUTO_UNLK) {
	//	ngxData.PED_annunAUTO_UNLK = data.PED_annunAUTO_UNLK;
	//	js["PED_annunAUTO_UNLK"] = data.PED_annunAUTO_UNLK;
	//}
	//if (ngxData.DOOR_annunFWD_ENTRY != data.DOOR_annunFWD_ENTRY) {
	//	ngxData.DOOR_annunFWD_ENTRY = data.DOOR_annunFWD_ENTRY;
	//	js["DOOR_annunFWD_ENTRY"] = data.DOOR_annunFWD_ENTRY;
	//}
	//if (ngxData.DOOR_annunFWD_SERVICE != data.DOOR_annunFWD_SERVICE) {
	//	ngxData.DOOR_annunFWD_SERVICE = data.DOOR_annunFWD_SERVICE;
	//	js["DOOR_annunFWD_SERVICE"] = data.DOOR_annunFWD_SERVICE;
	//}
	//if (ngxData.DOOR_annunAIRSTAIR != data.DOOR_annunAIRSTAIR) {
	//	ngxData.DOOR_annunAIRSTAIR = data.DOOR_annunAIRSTAIR;
	//	js["DOOR_annunAIRSTAIR"] = data.DOOR_annunAIRSTAIR;
	//}
	//if (ngxData.DOOR_annunLEFT_FWD_OVERWING != data.DOOR_annunLEFT_FWD_OVERWING) {
	//	ngxData.DOOR_annunLEFT_FWD_OVERWING = data.DOOR_annunLEFT_FWD_OVERWING;
	//	js["DOOR_annunLEFT_FWD_OVERWING"] = data.DOOR_annunLEFT_FWD_OVERWING;
	//}
	//if (ngxData.DOOR_annunRIGHT_FWD_OVERWING != data.DOOR_annunRIGHT_FWD_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_FWD_OVERWING = data.DOOR_annunRIGHT_FWD_OVERWING;
	//	js["DOOR_annunRIGHT_FWD_OVERWING"] = data.DOOR_annunRIGHT_FWD_OVERWING;
	//}
	//if (ngxData.DOOR_annunFWD_CARGO != data.DOOR_annunFWD_CARGO) {
	//	ngxData.DOOR_annunFWD_CARGO = data.DOOR_annunFWD_CARGO;
	//	js["DOOR_annunFWD_CARGO"] = data.DOOR_annunFWD_CARGO;
	//}
	//if (ngxData.DOOR_annunEQUIP != data.DOOR_annunEQUIP) {
	//	ngxData.DOOR_annunEQUIP = data.DOOR_annunEQUIP;
	//	js["DOOR_annunEQUIP"] = data.DOOR_annunEQUIP;
	//}
	//if (ngxData.DOOR_annunLEFT_AFT_OVERWING != data.DOOR_annunLEFT_AFT_OVERWING) {
	//	ngxData.DOOR_annunLEFT_AFT_OVERWING = data.DOOR_annunLEFT_AFT_OVERWING;
	//	js["DOOR_annunLEFT_AFT_OVERWING"] = data.DOOR_annunLEFT_AFT_OVERWING;
	//}
	//if (ngxData.DOOR_annunRIGHT_AFT_OVERWING != data.DOOR_annunRIGHT_AFT_OVERWING) {
	//	ngxData.DOOR_annunRIGHT_AFT_OVERWING = data.DOOR_annunRIGHT_AFT_OVERWING;
	//	js["DOOR_annunRIGHT_AFT_OVERWING"] = data.DOOR_annunRIGHT_AFT_OVERWING;
	//}
	//if (ngxData.DOOR_annunAFT_CARGO != data.DOOR_annunAFT_CARGO) {
	//	ngxData.DOOR_annunAFT_CARGO = data.DOOR_annunAFT_CARGO;
	//	js["DOOR_annunAFT_CARGO"] = data.DOOR_annunAFT_CARGO;
	//}
	//if (ngxData.DOOR_annunAFT_ENTRY != data.DOOR_annunAFT_ENTRY) {
	//	ngxData.DOOR_annunAFT_ENTRY = data.DOOR_annunAFT_ENTRY;
	//	js["DOOR_annunAFT_ENTRY"] = data.DOOR_annunAFT_ENTRY;
	//}
	//if (ngxData.DOOR_annunAFT_SERVICE != data.DOOR_annunAFT_SERVICE) {
	//	ngxData.DOOR_annunAFT_SERVICE = data.DOOR_annunAFT_SERVICE;
	//	js["DOOR_annunAFT_SERVICE"] = data.DOOR_annunAFT_SERVICE;
	//}
	if (ngxData.AIR_annunAUTO_FAIL != data.AIR_annunAUTO_FAIL) {
		ngxData.AIR_annunAUTO_FAIL = data.AIR_annunAUTO_FAIL;
		js["AIR_annunAUTO_FAIL"] = data.AIR_annunAUTO_FAIL;
	}
	if (ngxData.AIR_annunOFFSCHED_DESCENT != data.AIR_annunOFFSCHED_DESCENT) {
		ngxData.AIR_annunOFFSCHED_DESCENT = data.AIR_annunOFFSCHED_DESCENT;
		js["AIR_annunOFFSCHED_DESCENT"] = data.AIR_annunOFFSCHED_DESCENT;
	}
	if (ngxData.AIR_annunALTN != data.AIR_annunALTN) {
		ngxData.AIR_annunALTN = data.AIR_annunALTN;
		js["AIR_annunALTN"] = data.AIR_annunALTN;
	}
	if (ngxData.AIR_annunMANUAL != data.AIR_annunMANUAL) {
		ngxData.AIR_annunMANUAL = data.AIR_annunMANUAL;
		js["AIR_annunMANUAL"] = data.AIR_annunMANUAL;
	}



	//if (ngxData.IRS_annunALIGN[0] != data.IRS_annunALIGN[0]) {
	//	ngxData.IRS_annunALIGN[0] = data.IRS_annunALIGN[0];
	//	js["IRS_annunALIGN_1"] = data.IRS_annunALIGN[0];
	//}
	//if (ngxData.IRS_annunALIGN[1] != data.IRS_annunALIGN[1]) {
	//	ngxData.IRS_annunALIGN[1] = data.IRS_annunALIGN[1];
	//	js["IRS_annunALIGN_2"] = data.IRS_annunALIGN[1];
	//}
	//if (ngxData.IRS_annunON_DC[0] != data.IRS_annunON_DC[0]) {
	//	ngxData.IRS_annunON_DC[0] = data.IRS_annunON_DC[0];
	//	js["IRS_annunON_DC_1"] = data.IRS_annunON_DC[0];
	//}
	//if (ngxData.IRS_annunON_DC[1] != data.IRS_annunON_DC[1]) {
	//	ngxData.IRS_annunON_DC[1] = data.IRS_annunON_DC[1];
	//	js["IRS_annunON_DC_2"] = data.IRS_annunON_DC[1];
	//}
	//if (ngxData.IRS_annunFAULT[0] != data.IRS_annunFAULT[0]) {
	//	ngxData.IRS_annunFAULT[0] = data.IRS_annunFAULT[0];
	//	js["IRS_annunFAULT_1"] = data.IRS_annunFAULT[0];
	//}
	//if (ngxData.IRS_annunFAULT[1] != data.IRS_annunFAULT[1]) {
	//	ngxData.IRS_annunFAULT[1] = data.IRS_annunFAULT[1];
	//	js["IRS_annunFAULT_2"] = data.IRS_annunFAULT[1];
	//}
	//if (ngxData.IRS_annunDC_FAIL[0] != data.IRS_annunDC_FAIL[0]) {
	//	ngxData.IRS_annunDC_FAIL[0] = data.IRS_annunDC_FAIL[0];
	//	js["IRS_annunDC_FAIL_1"] = data.IRS_annunDC_FAIL[0];
	//}
	//if (ngxData.IRS_annunDC_FAIL[1] != data.IRS_annunDC_FAIL[1]) {
	//	ngxData.IRS_annunDC_FAIL[1] = data.IRS_annunDC_FAIL[1];
	//	js["IRS_annunDC_FAIL_2"] = data.IRS_annunDC_FAIL[1];
	//}
	if (ngxData.ENG_annunREVERSER[0] != data.ENG_annunREVERSER[0]) {
		ngxData.ENG_annunREVERSER[0] = data.ENG_annunREVERSER[0];
		js["ENG_annunREVERSER_1"] = data.ENG_annunREVERSER[0];
	}
	if (ngxData.ENG_annunREVERSER[1] != data.ENG_annunREVERSER[1]) {
		ngxData.ENG_annunREVERSER[1] = data.ENG_annunREVERSER[1];
		js["ENG_annunREVERSER_2"] = data.ENG_annunREVERSER[1];
	}
	if (ngxData.ENG_annunENGINE_CONTROL[0] != data.ENG_annunENGINE_CONTROL[0]) {
		ngxData.ENG_annunENGINE_CONTROL[0] = data.ENG_annunENGINE_CONTROL[0];
		js["ENG_annunENGINE_CONTROL_1"] = data.ENG_annunENGINE_CONTROL[0];
	}
	if (ngxData.ENG_annunENGINE_CONTROL[1] != data.ENG_annunENGINE_CONTROL[1]) {
		ngxData.ENG_annunENGINE_CONTROL[1] = data.ENG_annunENGINE_CONTROL[1];
		js["ENG_annunENGINE_CONTROL_2"] = data.ENG_annunENGINE_CONTROL[1];
	}
	//if (ngxData.ENG_annunALTN[0] != data.ENG_annunALTN[0]) {
	//	ngxData.ENG_annunALTN[0] = data.ENG_annunALTN[0];
	//	js["ENG_annunALTN_1"] = data.ENG_annunALTN[0];
	//}
	//if (ngxData.ENG_annunALTN[1] != data.ENG_annunALTN[1]) {
	//	ngxData.ENG_annunALTN[1] = data.ENG_annunALTN[1];
	//	js["ENG_annunALTN_2"] = data.ENG_annunALTN[1];
	//}
	if (ngxData.FCTL_annunFC_LOW_PRESSURE[0] != data.FCTL_annunFC_LOW_PRESSURE[0]) {
		ngxData.FCTL_annunFC_LOW_PRESSURE[0] = data.FCTL_annunFC_LOW_PRESSURE[0];
		js["FCTL_annunFC_LOW_PRESSURE_1"] = data.FCTL_annunFC_LOW_PRESSURE[0];
	}
	if (ngxData.FCTL_annunFC_LOW_PRESSURE[1] != data.FCTL_annunFC_LOW_PRESSURE[1]) {
		ngxData.FCTL_annunFC_LOW_PRESSURE[1] = data.FCTL_annunFC_LOW_PRESSURE[1];
		js["FCTL_annunFC_LOW_PRESSURE_2"] = data.FCTL_annunFC_LOW_PRESSURE[1];
	}
	if (ngxData.FUEL_annunENG_VALVE_CLOSED[0] != data.FUEL_annunENG_VALVE_CLOSED[0]) {
		ngxData.FUEL_annunENG_VALVE_CLOSED[0] = data.FUEL_annunENG_VALVE_CLOSED[0];
#ifdef PMDG_737_NG3 
		js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0] != 0;
#else
		js["FUEL_annunENG_VALVE_CLOSED_1"] = data.FUEL_annunENG_VALVE_CLOSED[0];

#endif
	}
	if (ngxData.FUEL_annunENG_VALVE_CLOSED[1] != data.FUEL_annunENG_VALVE_CLOSED[1]) {
		ngxData.FUEL_annunENG_VALVE_CLOSED[1] = data.FUEL_annunENG_VALVE_CLOSED[1];
#ifdef PMDG_737_NG3 
		js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1] != 0;
#else
		js["FUEL_annunENG_VALVE_CLOSED_2"] = data.FUEL_annunENG_VALVE_CLOSED[1];
#endif
	}
	if (ngxData.FUEL_annunSPAR_VALVE_CLOSED[0] != data.FUEL_annunSPAR_VALVE_CLOSED[0]) {
		ngxData.FUEL_annunSPAR_VALVE_CLOSED[0] = data.FUEL_annunSPAR_VALVE_CLOSED[0];
#ifdef PMDG_737_NG3 
		js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0] != 0;
#else
		js["FUEL_annunSPAR_VALVE_CLOSED_1"] = data.FUEL_annunSPAR_VALVE_CLOSED[0];
#endif
	}
	if (ngxData.FUEL_annunSPAR_VALVE_CLOSED[1] != data.FUEL_annunSPAR_VALVE_CLOSED[1]) {
		ngxData.FUEL_annunSPAR_VALVE_CLOSED[1] = data.FUEL_annunSPAR_VALVE_CLOSED[1];
#ifdef PMDG_737_NG3 
		js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1] != 0;
#else
		js["FUEL_annunSPAR_VALVE_CLOSED_2"] = data.FUEL_annunSPAR_VALVE_CLOSED[1];
#endif
	}
	if (ngxData.FUEL_annunFILTER_BYPASS[0] != data.FUEL_annunFILTER_BYPASS[0]) {
		ngxData.FUEL_annunFILTER_BYPASS[0] = data.FUEL_annunFILTER_BYPASS[0];
		js["FUEL_annunFILTER_BYPASS_1"] = data.FUEL_annunFILTER_BYPASS[0];
	}
	if (ngxData.FUEL_annunFILTER_BYPASS[1] != data.FUEL_annunFILTER_BYPASS[1]) {
		ngxData.FUEL_annunFILTER_BYPASS[1] = data.FUEL_annunFILTER_BYPASS[1];
		js["FUEL_annunFILTER_BYPASS_2"] = data.FUEL_annunFILTER_BYPASS[1];
	}
	if (ngxData.FUEL_annunLOWPRESS_Fwd[0] != data.FUEL_annunLOWPRESS_Fwd[0]) {
		ngxData.FUEL_annunLOWPRESS_Fwd[0] = data.FUEL_annunLOWPRESS_Fwd[0];
		js["FUEL_annunLOWPRESS_Fwd_1"] = data.FUEL_annunLOWPRESS_Fwd[0];
	}
	if (ngxData.FUEL_annunLOWPRESS_Fwd[1] != data.FUEL_annunLOWPRESS_Fwd[1]) {
		ngxData.FUEL_annunLOWPRESS_Fwd[1] = data.FUEL_annunLOWPRESS_Fwd[1];
		js["FUEL_annunLOWPRESS_Fwd_2"] = data.FUEL_annunLOWPRESS_Fwd[1];
	}
	if (ngxData.FUEL_annunLOWPRESS_Aft[0] != data.FUEL_annunLOWPRESS_Aft[0]) {
		ngxData.FUEL_annunLOWPRESS_Aft[0] = data.FUEL_annunLOWPRESS_Aft[0];
		js["FUEL_annunLOWPRESS_Aft_1"] = data.FUEL_annunLOWPRESS_Aft[0];
	}
	if (ngxData.FUEL_annunLOWPRESS_Aft[1] != data.FUEL_annunLOWPRESS_Aft[1]) {
		ngxData.FUEL_annunLOWPRESS_Aft[1] = data.FUEL_annunLOWPRESS_Aft[1];
		js["FUEL_annunLOWPRESS_Aft_2"] = data.FUEL_annunLOWPRESS_Aft[1];
	}
	if (ngxData.FUEL_annunLOWPRESS_Ctr[0] != data.FUEL_annunLOWPRESS_Ctr[0]) {
		ngxData.FUEL_annunLOWPRESS_Ctr[0] = data.FUEL_annunLOWPRESS_Ctr[0];
		js["FUEL_annunLOWPRESS_Ctr_1"] = data.FUEL_annunLOWPRESS_Ctr[0];
	}
	if (ngxData.FUEL_annunLOWPRESS_Ctr[1] != data.FUEL_annunLOWPRESS_Ctr[1]) {
		ngxData.FUEL_annunLOWPRESS_Ctr[1] = data.FUEL_annunLOWPRESS_Ctr[1];
		js["FUEL_annunLOWPRESS_Ctr_2"] = data.FUEL_annunLOWPRESS_Ctr[1];
	}
	if (ngxData.ELEC_annunDRIVE[0] != data.ELEC_annunDRIVE[0]) {
		ngxData.ELEC_annunDRIVE[0] = data.ELEC_annunDRIVE[0];
		js["ELEC_annunDRIVE_1"] = data.ELEC_annunDRIVE[0];
	}
	if (ngxData.ELEC_annunDRIVE[1] != data.ELEC_annunDRIVE[1]) {
		ngxData.ELEC_annunDRIVE[1] = data.ELEC_annunDRIVE[1];
		js["ELEC_annunDRIVE_2"] = data.ELEC_annunDRIVE[1];
	}
	if (ngxData.ELEC_annunTRANSFER_BUS_OFF[0] != data.ELEC_annunTRANSFER_BUS_OFF[0]) {
		ngxData.ELEC_annunTRANSFER_BUS_OFF[0] = data.ELEC_annunTRANSFER_BUS_OFF[0];
		js["ELEC_annunTRANSFER_BUS_OFF_1"] = data.ELEC_annunTRANSFER_BUS_OFF[0];
	}
	if (ngxData.ELEC_annunTRANSFER_BUS_OFF[1] != data.ELEC_annunTRANSFER_BUS_OFF[1]) {
		ngxData.ELEC_annunTRANSFER_BUS_OFF[1] = data.ELEC_annunTRANSFER_BUS_OFF[1];
		js["ELEC_annunTRANSFER_BUS_OFF_2"] = data.ELEC_annunTRANSFER_BUS_OFF[1];
	}
	if (ngxData.ELEC_annunSOURCE_OFF[0] != data.ELEC_annunSOURCE_OFF[0]) {
		ngxData.ELEC_annunSOURCE_OFF[0] = data.ELEC_annunSOURCE_OFF[0];
		js["ELEC_annunSOURCE_OFF_1"] = data.ELEC_annunSOURCE_OFF[0];
	}
	if (ngxData.ELEC_annunSOURCE_OFF[1] != data.ELEC_annunSOURCE_OFF[1]) {
		ngxData.ELEC_annunSOURCE_OFF[1] = data.ELEC_annunSOURCE_OFF[1];
		js["ELEC_annunSOURCE_OFF_2"] = data.ELEC_annunSOURCE_OFF[1];
	}
	if (ngxData.ELEC_annunGEN_BUS_OFF[0] != data.ELEC_annunGEN_BUS_OFF[0]) {
		ngxData.ELEC_annunGEN_BUS_OFF[0] = data.ELEC_annunGEN_BUS_OFF[0];
		js["ELEC_annunGEN_BUS_OFF_1"] = data.ELEC_annunGEN_BUS_OFF[0];
	}
	if (ngxData.ELEC_annunGEN_BUS_OFF[1] != data.ELEC_annunGEN_BUS_OFF[1]) {
		ngxData.ELEC_annunGEN_BUS_OFF[1] = data.ELEC_annunGEN_BUS_OFF[1];
		js["ELEC_annunGEN_BUS_OFF_2"] = data.ELEC_annunGEN_BUS_OFF[1];
	}
	if (ngxData.ICE_annunOVERHEAT[0] != data.ICE_annunOVERHEAT[0]) {
		ngxData.ICE_annunOVERHEAT[0] = data.ICE_annunOVERHEAT[0];
		js["ICE_annunOVERHEAT_1"] = data.ICE_annunOVERHEAT[0];
	}
	if (ngxData.ICE_annunOVERHEAT[1] != data.ICE_annunOVERHEAT[1]) {
		ngxData.ICE_annunOVERHEAT[1] = data.ICE_annunOVERHEAT[1];
		js["ICE_annunOVERHEAT_2"] = data.ICE_annunOVERHEAT[1];
	}
	if (ngxData.ICE_annunOVERHEAT[2] != data.ICE_annunOVERHEAT[2]) {
		ngxData.ICE_annunOVERHEAT[2] = data.ICE_annunOVERHEAT[2];
		js["ICE_annunOVERHEAT_3"] = data.ICE_annunOVERHEAT[2];
	}
	if (ngxData.ICE_annunOVERHEAT[3] != data.ICE_annunOVERHEAT[3]) {
		ngxData.ICE_annunOVERHEAT[3] = data.ICE_annunOVERHEAT[3];
		js["ICE_annunOVERHEAT_4"] = data.ICE_annunOVERHEAT[3];
	}
	if (ngxData.ICE_annunON[0] != data.ICE_annunON[0]) {
		ngxData.ICE_annunON[0] = data.ICE_annunON[0];
		js["ICE_annunON_1"] = data.ICE_annunON[0];
	}
	if (ngxData.ICE_annunON[1] != data.ICE_annunON[1]) {
		ngxData.ICE_annunON[1] = data.ICE_annunON[1];
		js["ICE_annunON_2"] = data.ICE_annunON[1];
	}
	if (ngxData.ICE_annunON[2] != data.ICE_annunON[2]) {
		ngxData.ICE_annunON[2] = data.ICE_annunON[2];
		js["ICE_annunON_3"] = data.ICE_annunON[2];
	}
	if (ngxData.ICE_annunON[3] != data.ICE_annunON[3]) {
		ngxData.ICE_annunON[3] = data.ICE_annunON[3];
		js["ICE_annunON_4"] = data.ICE_annunON[3];
	}
	if (ngxData.ICE_annunVALVE_OPEN[0] != data.ICE_annunVALVE_OPEN[0]) {
		ngxData.ICE_annunVALVE_OPEN[0] = data.ICE_annunVALVE_OPEN[0];
		js["ICE_annunVALVE_OPEN_1"] = data.ICE_annunVALVE_OPEN[0];
	}
	if (ngxData.ICE_annunVALVE_OPEN[1] != data.ICE_annunVALVE_OPEN[1]) {
		ngxData.ICE_annunVALVE_OPEN[1] = data.ICE_annunVALVE_OPEN[1];
		js["ICE_annunVALVE_OPEN_2"] = data.ICE_annunVALVE_OPEN[1];
	}
	if (ngxData.ICE_annunCOWL_ANTI_ICE[0] != data.ICE_annunCOWL_ANTI_ICE[0]) {
		ngxData.ICE_annunCOWL_ANTI_ICE[0] = data.ICE_annunCOWL_ANTI_ICE[0];
		js["ICE_annunCOWL_ANTI_ICE_1"] = data.ICE_annunCOWL_ANTI_ICE[0];
	}
	if (ngxData.ICE_annunCOWL_ANTI_ICE[1] != data.ICE_annunCOWL_ANTI_ICE[1]) {
		ngxData.ICE_annunCOWL_ANTI_ICE[1] = data.ICE_annunCOWL_ANTI_ICE[1];
		js["ICE_annunCOWL_ANTI_ICE_2"] = data.ICE_annunCOWL_ANTI_ICE[1];
	}
	if (ngxData.ICE_annunCOWL_VALVE_OPEN[0] != data.ICE_annunCOWL_VALVE_OPEN[0]) {
		ngxData.ICE_annunCOWL_VALVE_OPEN[0] = data.ICE_annunCOWL_VALVE_OPEN[0];
		js["ICE_annunCOWL_VALVE_OPEN_1"] = data.ICE_annunCOWL_VALVE_OPEN[0];
	}
	if (ngxData.ICE_annunCOWL_VALVE_OPEN[1] != data.ICE_annunCOWL_VALVE_OPEN[1]) {
		ngxData.ICE_annunCOWL_VALVE_OPEN[1] = data.ICE_annunCOWL_VALVE_OPEN[1];
		js["ICE_annunCOWL_VALVE_OPEN_2"] = data.ICE_annunCOWL_VALVE_OPEN[1];
	}
	if (ngxData.HYD_annunLOW_PRESS_eng[0] != data.HYD_annunLOW_PRESS_eng[0]) {
		ngxData.HYD_annunLOW_PRESS_eng[0] = data.HYD_annunLOW_PRESS_eng[0];
		js["HYD_annunLOW_PRESS_eng_1"] = data.HYD_annunLOW_PRESS_eng[0];
	}
	if (ngxData.HYD_annunLOW_PRESS_eng[1] != data.HYD_annunLOW_PRESS_eng[1]) {
		ngxData.HYD_annunLOW_PRESS_eng[1] = data.HYD_annunLOW_PRESS_eng[1];
		js["HYD_annunLOW_PRESS_eng_2"] = data.HYD_annunLOW_PRESS_eng[1];
	}
	if (ngxData.HYD_annunLOW_PRESS_elec[0] != data.HYD_annunLOW_PRESS_elec[0]) {
		ngxData.HYD_annunLOW_PRESS_elec[0] = data.HYD_annunLOW_PRESS_elec[0];
		js["HYD_annunLOW_PRESS_elec_1"] = data.HYD_annunLOW_PRESS_elec[0];
	}
	if (ngxData.HYD_annunLOW_PRESS_elec[1] != data.HYD_annunLOW_PRESS_elec[1]) {
		ngxData.HYD_annunLOW_PRESS_elec[1] = data.HYD_annunLOW_PRESS_elec[1];
		js["HYD_annunLOW_PRESS_elec_2"] = data.HYD_annunLOW_PRESS_elec[1];
	}
	if (ngxData.HYD_annunOVERHEAT_elec[0] != data.HYD_annunOVERHEAT_elec[0]) {
		ngxData.HYD_annunOVERHEAT_elec[0] = data.HYD_annunOVERHEAT_elec[0];
		js["HYD_annunOVERHEAT_elec_1"] = data.HYD_annunOVERHEAT_elec[0];
	}
	if (ngxData.HYD_annunOVERHEAT_elec[1] != data.HYD_annunOVERHEAT_elec[1]) {
		ngxData.HYD_annunOVERHEAT_elec[1] = data.HYD_annunOVERHEAT_elec[1];
		js["HYD_annunOVERHEAT_elec_2"] = data.HYD_annunOVERHEAT_elec[1];
	}
	if (ngxData.AIR_annunZoneTemp[0] != data.AIR_annunZoneTemp[0]) {
		ngxData.AIR_annunZoneTemp[0] = data.AIR_annunZoneTemp[0];
		js["AIR_annunZoneTemp_1"] = data.AIR_annunZoneTemp[0];
	}
	if (ngxData.AIR_annunZoneTemp[1] != data.AIR_annunZoneTemp[1]) {
		ngxData.AIR_annunZoneTemp[1] = data.AIR_annunZoneTemp[1];
		js["AIR_annunZoneTemp_2"] = data.AIR_annunZoneTemp[1];
	}
	if (ngxData.AIR_annunZoneTemp[2] != data.AIR_annunZoneTemp[2]) {
		ngxData.AIR_annunZoneTemp[2] = data.AIR_annunZoneTemp[2];
		js["AIR_annunZoneTemp_3"] = data.AIR_annunZoneTemp[2];
	}
	if (ngxData.AIR_annunPackTripOff[0] != data.AIR_annunPackTripOff[0]) {
		ngxData.AIR_annunPackTripOff[0] = data.AIR_annunPackTripOff[0];
		js["AIR_annunPackTripOff_1"] = data.AIR_annunPackTripOff[0];
	}
	if (ngxData.AIR_annunPackTripOff[1] != data.AIR_annunPackTripOff[1]) {
		ngxData.AIR_annunPackTripOff[1] = data.AIR_annunPackTripOff[1];
		js["AIR_annunPackTripOff_2"] = data.AIR_annunPackTripOff[1];
	}
	if (ngxData.AIR_annunWingBodyOverheat[0] != data.AIR_annunWingBodyOverheat[0]) {
		ngxData.AIR_annunWingBodyOverheat[0] = data.AIR_annunWingBodyOverheat[0];
		js["AIR_annunWingBodyOverheat_1"] = data.AIR_annunWingBodyOverheat[0];
	}
	if (ngxData.AIR_annunWingBodyOverheat[1] != data.AIR_annunWingBodyOverheat[1]) {
		ngxData.AIR_annunWingBodyOverheat[1] = data.AIR_annunWingBodyOverheat[1];
		js["AIR_annunWingBodyOverheat_2"] = data.AIR_annunWingBodyOverheat[1];
	}
	if (ngxData.AIR_annunBleedTripOff[0] != data.AIR_annunBleedTripOff[0]) {
		ngxData.AIR_annunBleedTripOff[0] = data.AIR_annunBleedTripOff[0];
		js["AIR_annunBleedTripOff_1"] = data.AIR_annunBleedTripOff[0];
	}
	if (ngxData.AIR_annunBleedTripOff[1] != data.AIR_annunBleedTripOff[1]) {
		ngxData.AIR_annunBleedTripOff[1] = data.AIR_annunBleedTripOff[1];
		js["AIR_annunBleedTripOff_2"] = data.AIR_annunBleedTripOff[1];
	}
	//if (ngxData.WARN_annunFIRE_WARN[0] != data.WARN_annunFIRE_WARN[0]) {
	//	ngxData.WARN_annunFIRE_WARN[0] = data.WARN_annunFIRE_WARN[0];
	//	js["WARN_annunFIRE_WARN_1"] = data.WARN_annunFIRE_WARN[0];
	//}
	//if (ngxData.WARN_annunFIRE_WARN[1] != data.WARN_annunFIRE_WARN[1]) {
	//	ngxData.WARN_annunFIRE_WARN[1] = data.WARN_annunFIRE_WARN[1];
	//	js["WARN_annunFIRE_WARN_2"] = data.WARN_annunFIRE_WARN[1];
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[0] != data.WARN_annunMASTER_CAUTION[0]) {
	//	ngxData.WARN_annunMASTER_CAUTION[0] = data.WARN_annunMASTER_CAUTION[0];
	//	js["WARN_annunMASTER_CAUTION_1"] = data.WARN_annunMASTER_CAUTION[0];
	//}
	//if (ngxData.WARN_annunMASTER_CAUTION[1] != data.WARN_annunMASTER_CAUTION[1]) {
	//	ngxData.WARN_annunMASTER_CAUTION[1] = data.WARN_annunMASTER_CAUTION[1];
	//	js["WARN_annunMASTER_CAUTION_2"] = data.WARN_annunMASTER_CAUTION[1];
	//}
	//if (ngxData.MAIN_annunBELOW_GS[0] != data.MAIN_annunBELOW_GS[0]) {
	//	ngxData.MAIN_annunBELOW_GS[0] = data.MAIN_annunBELOW_GS[0];
	//	js["MAIN_annunBELOW_GS_1"] = data.MAIN_annunBELOW_GS[0];
	//}
	//if (ngxData.MAIN_annunBELOW_GS[1] != data.MAIN_annunBELOW_GS[1]) {
	//	ngxData.MAIN_annunBELOW_GS[1] = data.MAIN_annunBELOW_GS[1];
	//	js["MAIN_annunBELOW_GS_2"] = data.MAIN_annunBELOW_GS[1];
	//}
	//if (ngxData.MAIN_annunAP[0] != data.MAIN_annunAP[0]) {
	//	ngxData.MAIN_annunAP[0] = data.MAIN_annunAP[0];
	//	js["MAIN_annunAP_1"] = data.MAIN_annunAP[0];
	//}
	//if (ngxData.MAIN_annunAP[1] != data.MAIN_annunAP[1]) {
	//	ngxData.MAIN_annunAP[1] = data.MAIN_annunAP[1];
	//	js["MAIN_annunAP_2"] = data.MAIN_annunAP[1];
	//}
	//if (ngxData.MAIN_annunAT[0] != data.MAIN_annunAT[0]) {
	//	ngxData.MAIN_annunAT[0] = data.MAIN_annunAT[0];
	//	js["MAIN_annunAT_1"] = data.MAIN_annunAT[0];
	//}
	//if (ngxData.MAIN_annunAT[1] != data.MAIN_annunAT[1]) {
	//	ngxData.MAIN_annunAT[1] = data.MAIN_annunAT[1];
	//	js["MAIN_annunAT_2"] = data.MAIN_annunAT[1];
	//}
	//if (ngxData.MAIN_annunFMC[0] != data.MAIN_annunFMC[0]) {
	//	ngxData.MAIN_annunFMC[0] = data.MAIN_annunFMC[0];
	//	js["MAIN_annunFMC_1"] = data.MAIN_annunFMC[0];
	//}
	//if (ngxData.MAIN_annunFMC[1] != data.MAIN_annunFMC[1]) {
	//	ngxData.MAIN_annunFMC[1] = data.MAIN_annunFMC[1];
	//	js["MAIN_annunFMC_2"] = data.MAIN_annunFMC[1];
	//}
	//if (ngxData.MAIN_annunGEAR_transit[0] != data.MAIN_annunGEAR_transit[0]) {
	//	ngxData.MAIN_annunGEAR_transit[0] = data.MAIN_annunGEAR_transit[0];
	//	js["MAIN_annunGEAR_transit_1"] = data.MAIN_annunGEAR_transit[0];
	//}
	//if (ngxData.MAIN_annunGEAR_transit[1] != data.MAIN_annunGEAR_transit[1]) {
	//	ngxData.MAIN_annunGEAR_transit[1] = data.MAIN_annunGEAR_transit[1];
	//	js["MAIN_annunGEAR_transit_2"] = data.MAIN_annunGEAR_transit[1];
	//}
	//if (ngxData.MAIN_annunGEAR_transit[2] != data.MAIN_annunGEAR_transit[2]) {
	//	ngxData.MAIN_annunGEAR_transit[2] = data.MAIN_annunGEAR_transit[2];
	//	js["MAIN_annunGEAR_transit_3"] = data.MAIN_annunGEAR_transit[2];
	//}

	if (ngxData.MAIN_annunGEAR_locked[0] != data.MAIN_annunGEAR_locked[0]) {
		ngxData.MAIN_annunGEAR_locked[0] = data.MAIN_annunGEAR_locked[0];
		js["MAIN_annunGEAR_locked_1"] = data.MAIN_annunGEAR_locked[0];
	}
	if (ngxData.MAIN_annunGEAR_locked[1] != data.MAIN_annunGEAR_locked[1]) {
		ngxData.MAIN_annunGEAR_locked[1] = data.MAIN_annunGEAR_locked[1];
		js["MAIN_annunGEAR_locked_2"] = data.MAIN_annunGEAR_locked[1];
	}
	if (ngxData.MAIN_annunGEAR_locked[2] != data.MAIN_annunGEAR_locked[2]) {
		ngxData.MAIN_annunGEAR_locked[2] = data.MAIN_annunGEAR_locked[2];
		js["MAIN_annunGEAR_locked_3"] = data.MAIN_annunGEAR_locked[2];
	}

	//if (ngxData.FIRE_annunENG_OVERHEAT[0] != data.FIRE_annunENG_OVERHEAT[0]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[0] = data.FIRE_annunENG_OVERHEAT[0];
	//	js["FIRE_annunENG_OVERHEAT_1"] = data.FIRE_annunENG_OVERHEAT[0];
	//}
	//if (ngxData.FIRE_annunENG_OVERHEAT[1] != data.FIRE_annunENG_OVERHEAT[1]) {
	//	ngxData.FIRE_annunENG_OVERHEAT[1] = data.FIRE_annunENG_OVERHEAT[1];
	//	js["FIRE_annunENG_OVERHEAT_2"] = data.FIRE_annunENG_OVERHEAT[1];
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[0] != data.FIRE_annunBOTTLE_DISCHARGE[0]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[0] = data.FIRE_annunBOTTLE_DISCHARGE[0];
	//	js["FIRE_annunBOTTLE_DISCHARGE_1"] = data.FIRE_annunBOTTLE_DISCHARGE[0];
	//}
	//if (ngxData.FIRE_annunBOTTLE_DISCHARGE[1] != data.FIRE_annunBOTTLE_DISCHARGE[1]) {
	//	ngxData.FIRE_annunBOTTLE_DISCHARGE[1] = data.FIRE_annunBOTTLE_DISCHARGE[1];
	//	js["FIRE_annunBOTTLE_DISCHARGE_2"] = data.FIRE_annunBOTTLE_DISCHARGE[1];
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[0] != data.FIRE_annunExtinguisherTest[0]) {
	//	ngxData.FIRE_annunExtinguisherTest[0] = data.FIRE_annunExtinguisherTest[0];
	//	js["FIRE_annunExtinguisherTest_1"] = data.FIRE_annunExtinguisherTest[0];
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[1] != data.FIRE_annunExtinguisherTest[1]) {
	//	ngxData.FIRE_annunExtinguisherTest[1] = data.FIRE_annunExtinguisherTest[1];
	//	js["FIRE_annunExtinguisherTest_2"] = data.FIRE_annunExtinguisherTest[1];
	//}
	//if (ngxData.FIRE_annunExtinguisherTest[2] != data.FIRE_annunExtinguisherTest[2]) {
	//	ngxData.FIRE_annunExtinguisherTest[2] = data.FIRE_annunExtinguisherTest[2];
	//	js["FIRE_annunExtinguisherTest_3"] = data.FIRE_annunExtinguisherTest[2];
	//}
	//if (ngxData.CARGO_annunExtTest[0] != data.CARGO_annunExtTest[0]) {
	//	ngxData.CARGO_annunExtTest[0] = data.CARGO_annunExtTest[0];
	//	js["CARGO_annunExtTest_1"] = data.CARGO_annunExtTest[0];
	//}
	//if (ngxData.CARGO_annunExtTest[1] != data.CARGO_annunExtTest[1]) {
	//	ngxData.CARGO_annunExtTest[1] = data.CARGO_annunExtTest[1];
	//	js["CARGO_annunExtTest_2"] = data.CARGO_annunExtTest[1];
	//}
	//if (ngxData.MAIN_annunAP_Amber[0] != data.MAIN_annunAP_Amber[0]) {
	//	ngxData.MAIN_annunAP_Amber[0] = data.MAIN_annunAP_Amber[0];
	//	js["MAIN_annunAP_Amber_1"] = data.MAIN_annunAP_Amber[0];
	//}
	//if (ngxData.MAIN_annunAP_Amber[1] != data.MAIN_annunAP_Amber[1]) {
	//	ngxData.MAIN_annunAP_Amber[1] = data.MAIN_annunAP_Amber[1];
	//	js["MAIN_annunAP_Amber_2"] = data.MAIN_annunAP_Amber[1];
	//}
	//if (ngxData.MAIN_annunAT_Amber[0] != data.MAIN_annunAT_Amber[0]) {
	//	ngxData.MAIN_annunAT_Amber[0] = data.MAIN_annunAT_Amber[0];
	//	js["MAIN_annunAT_Amber_1"] = data.MAIN_annunAT_Amber[0];
	//}
	//if (ngxData.MAIN_annunAT_Amber[1] != data.MAIN_annunAT_Amber[1]) {
	//	ngxData.MAIN_annunAT_Amber[1] = data.MAIN_annunAT_Amber[1];
	//	js["MAIN_annunAT_Amber_2"] = data.MAIN_annunAT_Amber[1];
	//}

	return js.dump();

}
