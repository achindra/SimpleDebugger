#pragma once

#ifndef _HEADER_
#define _HEADER_

#define BUFFER_SIZE 255

#define SD_LOG(fmt, ...)   printf(fmt, __VA_ARGS__)

#define SD_LOG_INFO(fmt, ...)   \
    printf("INFO: "); \
    printf(fmt, __VA_ARGS__);


#define SD_LOG_WARNING(fmt, ...)   \
    printf("WARNING: "); \
    printf(fmt, __VA_ARGS__);


#define SD_LOG_ERROR(fmt, ...)   \
    printf("ERROR: "); \
    printf(fmt, __VA_ARGS__);


class OutputCallbacks : public IDebugOutputCallbacks
{
    
public:
	// IUnknown.
    STDMETHOD(QueryInterface)(
        THIS_
        IN REFIID InterfaceId,
        OUT PVOID* Interface
        );

    STDMETHOD_(ULONG, AddRef)(
        THIS
        );

    STDMETHOD_(ULONG, Release)(
        THIS
        );

    // IDebugOutputCallbacks.
    
    // This method is only called if the supplied mask
    // is allowed by the clients output control.
    // The return value is ignored.
    STDMETHOD(Output)(
        THIS_
        IN ULONG Mask,
        IN PCSTR Text
        );
};

#endif