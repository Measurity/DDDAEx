// DDDAExHoster.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DDDAExHoster.h"

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int ExecuteCLR()
{
	HRESULT hr;
	ICLRMetaHost *pMetaHost = NULL;
	ICLRRuntimeInfo *pRuntimeInfo = NULL;
	ICLRRuntimeHost *pClrRuntimeHost = NULL;

	// build runtime
	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_PPV_ARGS(&pRuntimeInfo));
	hr = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost,
		IID_PPV_ARGS(&pClrRuntimeHost));

	// start runtime
	hr = pClrRuntimeHost->Start();

	// execute managed assembly
	DWORD pReturnValue;
	hr = pClrRuntimeHost->ExecuteInDefaultAppDomain(
		L"DDDAEx.exe",
		L"DDDAEx.Program",
		L"Init",
		L"Hello from injected DLL :)",
		&pReturnValue);

	// free resources
	pMetaHost->Release();
	pRuntimeInfo->Release();
	pClrRuntimeHost->Release();

	return 0;
}