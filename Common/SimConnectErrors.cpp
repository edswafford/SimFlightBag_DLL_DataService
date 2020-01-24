
#include <windows.h>
#include <tchar.h>
#include "SimConnect.h"
#include <map>
#include <string>


std::map< SIMCONNECT_EXCEPTION, std::string> SimConnectErrorMap{
{SIMCONNECT_EXCEPTION_NONE, "Specifies that there has not been an error. This value is not currently used."},
{SIMCONNECT_EXCEPTION_ERROR, "An unspecific error has occurred. This can be from incorrect flag settings, null or incorrect parameters, the need to have at least one up or down event with an input event, failed calls from the SimConnect server to the operating system, among other reasons."},
{SIMCONNECT_EXCEPTION_SIZE_MISMATCH, "Specifies the size of the data provided does not match the size required. This typically occurs when the wrong string length, fixed or variable, is involved."},
{SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID, "Specifies that the client event, request ID, data definition ID, or object ID was not recognized."},
{SIMCONNECT_EXCEPTION_UNOPENED, "Specifies that communication with the SimConnect server has not been opened. This error is not currently used."},
{SIMCONNECT_EXCEPTION_VERSION_MISMATCH, "Specifies a versioning error has occurred. Typically this will occur when a client built on a newer version of the SimConnect client dll attempts to work with an older version of the SimConnect server."},
{SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS, "Specifies that the maximum number of groups allowed has been reached. The maximum is 20."},
{SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED, "Specifies that the simulation event name (such as 'brakes') is not recognized."},
{SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES, "Specifies that the maximum number of event names allowed has been reached. The maximum is 1000."},
{SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE, "Specifies that the event ID has been used already. This can occur with calls to SimConnect_MapClientEventToSimEvent, or SimConnect_SubscribeToSystemEvent."},
{SIMCONNECT_EXCEPTION_TOO_MANY_MAPS, "Specifies that the maximum number of mappings allowed has been reached. The maximum is 20."},
{SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS, "Specifies that the maximum number of objects allowed has been reached. The maximum is 1000."},
{SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS, "Specifies that the maximum number of requests allowed has been reached. The maximum is 1000."},
{SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE, "Specifies that the data type requested does not apply to the type of data requested. Typically this occurs with a fixed length string of the wrong length."},
{SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE, "Specifies that the size of the data provided is not what is expected. This can occur when the size of a structure provided does not match the size given, or a null string entry is made for a menu or sub-menu entry text, or data with a size of zero is added to a data definition. It can also occur with an invalid request to SimConnect_CreateClientData."},
{SIMCONNECT_EXCEPTION_DATA_ERROR, "Specifies a generic data error. This error is used by the SimConnect_WeatherCreateThermal function to report incorrect parameters, such as negative radii or values greater than the maximum allowed. It is also used by the SimConnect_FlightSave and SimConnect_FlightLoad functions to report incorrect file types. It is also used by other functions to report that flags or reserved parameters have not been set to zero."},
{SIMCONNECT_EXCEPTION_INVALID_ARRAY, "Specifies an invalid array has been sent to the SimConnect_SetDataOnSimObject function."},
{SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED, "Specifies that the client has already subscribed to that event."},
{SIMCONNECT_EXCEPTION_INVALID_ENUM, "Specifies that the member of the enumeration provided was not valid. Currently this is only used if an unknown type is provided to SimConnect_RequestDataOnSimObjectType."},
{SIMCONNECT_EXCEPTION_DEFINITION_ERROR, "Specifies that there is a problem with a data definition. Currently this is only used if a variable length definition is sent with SimConnect_RequestDataOnSimObject."},
{SIMCONNECT_EXCEPTION_DUPLICATE_ID, "Specifies that the ID has already been used. This can occur with menu IDs, or with the IDs provided to SimConnect_AddToDataDefinition, SimConnect_AddClientEventToNotificationGroup or SimConnect_MapClientDataNameToID."},
{SIMCONNECT_EXCEPTION_DATUM_ID, "Specifies that the datum ID is not recognized. This currently occurs with a call to the SimConnect_SetDataOnSimObject function."},
{SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS, "Specifies that the radius given in the SimConnect_RequestDataOnSimObjectType was outside the acceptable range, or with an invalid request to SimConnect_CreateClientData."},
{SIMCONNECT_EXCEPTION_ALREADY_CREATED, "Specifies that a client data area with the name requested by a call to SimConnect_MapClientDataNameToID has already been created by another add-on. Try again with a different name."},
{SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT, "Specifies an invalid port number was requested."},
{SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR, "Specifies that the metar data supplied did not match the required format. See the section Metar Data Format for details on the format required."},
{SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION, "Specifies that the weather observation requested was not available. Refer to the remarks section for SimConnect_WeatherRequestObservationAtStation for some notes on this exception."},
{SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION, "Specifies that the weather station could not be created."},
{SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION, "Specifies that the weather station could not be removed."},
{SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED, "Specifies that the attempt to create an AI object failed."},
{SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED, "Specifies that the specified flight plan could not be found, or did not load correctly."},
{SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE, "Specifies that the operation requested does not apply to the object type, for example trying to set a flight plan on an object that is not an aircraft will result in this error."},
{SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION, "Specifies that the AI operation requested cannot be completed, such as requesting that an object be removed when the client did not create that object."},
{SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE, "Specifies that an attempt to create an ATC controlled AI object failed because the location of the object is outside the reality bubble."},
{SIMCONNECT_EXCEPTION_OBJECT_CONTAINER, "Specifies that an attempt to create an AI object failed because of an error with the container system for the object."},
{SIMCONNECT_EXCEPTION_OBJECT_AI, "Specifies that an attempt to create an AI object failed because of an error with the AI system for the object."},
{SIMCONNECT_EXCEPTION_OBJECT_ATC, "Specifies that an attempt to create an AI object failed because of an error with the ATC system for the object."},
{SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE, "Specifies that an attempt to create an AI object failed because of a scheduling problem."},
#ifndef FSX_FLIGHT_SIMULATOR_PROXY
	{SIMCONNECT_EXCEPTION_BLOCK_TIMEOUT, "Specifies that a synchronous blocking callback didn't release the block within the timeout period." }
#endif
};
