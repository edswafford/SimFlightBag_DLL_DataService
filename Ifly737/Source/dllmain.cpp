#include "framework.h"

#include <logger\logger.h>
#include "GenericSimConnect.h"

logger LOG("SimFlightBagServer.log");
//
// Used to send LOG messages to Windows --> SendMessage -- not needed in DLL
//
void OutputMessageToTextWindow(std::string message) {}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) ISimDataService *
__cdecl createSimDataConnection(std::shared_ptr<AsyncQueue<std::string>> const& dataQueue,
	std::shared_ptr<AsyncQueue<std::string>> const& msgQueue)
{
	return new GenericSimConnect(dataQueue, msgQueue, std::string("Starting IFly 737 NGX Data Service"));
}
