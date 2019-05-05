// SimpleDebugger.cpp : Defines the entry point for the console application.
//

//#pragma comment(lib, "dbgeng.lib")

#include "stdafx.h"
#include <Windows.h>
#include <WDBGEXTS.H>
#include <DbgEng.h>
#include "Header.h"

IDebugClient* g_Client;
IDebugControl* g_Control;
IDebugDataSpaces* g_Data;
IDebugSymbols* g_Symbols;

WINDBG_EXTENSION_APIS     ExtensionApis;

/*
 * Copied Code Below
 * //depot/win8_ldr/sdktools/checksym/src/checksym/dmpfile.cpp#1
 */


OutputCallbacks g_OutputCB;

STDMETHODIMP
OutputCallbacks::QueryInterface(
    THIS_
    IN REFIID InterfaceId,
    OUT PVOID* Interface
    )
{
    *Interface = NULL;

	if (IsEqualIID(InterfaceId,__uuidof(IUnknown)) ||
        IsEqualIID(InterfaceId,__uuidof(IDebugOutputCallbacks)))
    {
        *Interface = (IDebugOutputCallbacks *)this;
        AddRef();
        return S_OK;
    }
    else
    {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG)
OutputCallbacks::AddRef(
    THIS
    )
{
    // This class is designed to be static so
    // there's no true refcount.
    return 1;
}

STDMETHODIMP_(ULONG)
OutputCallbacks::Release(
    THIS
    )
{
    // This class is designed to be static so
    // there's no true refcount.
    return 0;
}

STDMETHODIMP
OutputCallbacks::Output(
    THIS_
    IN ULONG Mask,
    IN PCSTR Text
    )
{
    HRESULT Status = S_OK;

	// If the client has asked for any output... do so.
	SD_LOG(Text);
    return Status;
}

/* Copied Code End */


HRESULT CreateInterface()
{
	HRESULT hr = S_FALSE;
	
	if((hr = DebugCreate(__uuidof(IDebugClient),(PVOID*)&g_Client) != S_OK))
	{
		goto End;

	}

	if ((((hr = g_Client->QueryInterface(__uuidof(IDebugControl),(void**)&g_Control)) != S_OK) ||
	   (hr = g_Client->QueryInterface(__uuidof(IDebugSymbols),(void**)&g_Symbols)) != S_OK)   ||
	   (hr = g_Client->QueryInterface(__uuidof(IDebugDataSpaces),(void**)&g_Data)) != S_OK)
	{

		goto End;
	}

	ExtensionApis.nSize = sizeof (ExtensionApis);
	g_Control->GetWindbgExtensionApis64((PWINDBG_EXTENSION_APIS64)&ExtensionApis);
	

End:

	return hr;

}



int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr=S_OK;
	CHAR SrcDump[BUFFER_SIZE];

	strcpy_s(SrcDump,(size_t)BUFFER_SIZE,"C:\\Intel\\newdmp\\MEMORY.DMP");

	if((hr=CreateInterface())!=S_OK)
	{
		SD_LOG_ERROR("CreateInterface failed %d \n", hr);
		goto CleanUp;
	}

	
	g_Symbols->SetSymbolPath("Srv*C:\\Symbols*http://msdl.microsoft.com/download/symbols");
	

	if(S_OK!=(hr=g_Client->SetOutputCallbacks(&g_OutputCB)))
	{
		SD_LOG_ERROR("Failed to register output callback %x\n", hr);
		goto CleanUp;
	}
	
	if(S_OK!=(hr = g_Client->OpenDumpFile(SrcDump)))
	{
		SD_LOG_ERROR("Failed to open dump file %x\n", hr);
		goto CleanUp;
	}

	SD_LOG_INFO("Waiting for Symbol Load \n");
	if(S_OK!=(hr=g_Control->WaitForEvent(0, INFINITE)))// DEBUG_WAIT_DEFAULT, INFINITE)))
	{
		SD_LOG_ERROR("WaitForEvent Failed %x\n", hr);
		goto CleanUp;
	}

	g_Symbols->Reload("");
	
	ULONG64 PfnDatabaseAddr = GetExpression("nt!MmPfnDatabase");
	SD_LOG_INFO("PfnDatabaseAddr: %x\n", PfnDatabaseAddr);

	g_Control->Execute(DEBUG_OUTCTL_ALL_CLIENTS,"x nt!MmPfnDatabase", DEBUG_EXECUTE_NOT_LOGGED);

CleanUp:

	return 0;
}

