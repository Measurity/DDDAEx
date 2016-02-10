#pragma once
#include <metahost.h>

#pragma comment(lib, "mscoree.lib")

#import "mscorlib.tlb" raw_interfaces_only \
    high_property_prefixes("_get","_put","_putref") \
    rename("ReportEvent", "InteropServices_ReportEvent")
//typedef int(__stdcall *f_funci)();

class DDDAExHoster
{
public:
	static __declspec(dllexport) int executeCLR();
};

