#pragma comment(lib, "user32")
#pragma comment(lib, "Ole32")
#pragma comment(lib, "Advapi32")

#include <shobjidl_core.h>
#include "ConvertCommand.cpp"
#include "Registry.h"

// {CAC95FBF-2281-4E29-A6E8-FA7DDF1DB85B}
static const IID IID_ConvertCommand = { 0xcac95fbf, 0x2281, 0x4e29, { 0xa6, 0xe8, 0xfa, 0x7d, 0xdf, 0x1d, 0xb8, 0x5b } };

// {7F101FB8-F82A-49E8-BF5D-EB38F2C0521C}
static const CLSID CLSID_Component1 = { 0x7f101fb8, 0xf82a, 0x49e8, { 0xbf, 0x5d, 0xeb, 0x38, 0xf2, 0xc0, 0x52, 0x1c } };

static HMODULE g_hModule = NULL;   // DLL module handle
static long g_cComponents = 0;     // Count of active components
static long g_cServerLocks = 0;    // Count of locks
const char g_szFriendlyName[] = "ConvertCommand RAWtoJPEG";
const char g_szVerIndProgID[] = "rmoalic.RAWtoJPG";
const char g_szProgID[] = "rmoalic.RAWtoJPG.1";

class CFactory : public IClassFactory
{
public:
    // IUnknown
    HRESULT __stdcall CFactory::QueryInterface(const IID& iid, void** ppv) {
        if ((iid == IID_IUnknown) || (iid == IID_IClassFactory)) {
            *ppv = static_cast<IClassFactory*>(this);
        }
        else {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    ULONG __stdcall CFactory::AddRef() {
        return InterlockedIncrement(&m_cRef);
    }

    ULONG __stdcall CFactory::Release() {
        if (InterlockedDecrement(&m_cRef) == 0) {
            delete this;
            return 0;
        }
        return m_cRef;
    }

    // IClassFactory
    HRESULT __stdcall CFactory::CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv) {
        // Cannot aggregate.
        if (pUnknownOuter != NULL) {
            return CLASS_E_NOAGGREGATION;
        }

        // Create component.
        ConvertCommand* pA = new ConvertCommand;
        if (pA == NULL)
        {
            return E_OUTOFMEMORY;
        }

        // Get the requested interface.
        HRESULT hr = pA->QueryInterface(iid, ppv);

        // Release the IUnknown pointer.
        // (If QueryInterface failed, component will delete itself.)
        pA->Release();
        return hr;
    }

    // LockServer
    HRESULT __stdcall CFactory::LockServer(BOOL bLock) {
        if (bLock) {
            InterlockedIncrement(&g_cServerLocks);
        }
        else {
            InterlockedDecrement(&g_cServerLocks);
        }
        return S_OK;
    }

    CFactory() : m_cRef(1) {}

private:
    long m_cRef;
};

// DLL functions

STDAPI DllCanUnloadNow() {
    if ((g_cComponents == 0) && (g_cServerLocks == 0)) {
        return S_OK;
    }
    else {
        return S_FALSE;
    }
}


STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv) {
    // Can we create this component?
    if (clsid != CLSID_Component1) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    // Create class factory.
    CFactory* pFactory = new CFactory;  // Reference count set to 1
    // in constructor
    if (pFactory == NULL) {
        return E_OUTOFMEMORY;
    }

    // Get requested interface.
    HRESULT hr = pFactory->QueryInterface(iid, ppv);
    pFactory->Release();

    return hr;
}

STDAPI DllRegisterServer() {
    return RegisterServer(g_hModule,
        CLSID_Component1,
        g_szFriendlyName,
        g_szVerIndProgID,
        g_szProgID);
}


STDAPI DllUnregisterServer() {
    return UnregisterServer(CLSID_Component1,
        g_szVerIndProgID,
        g_szProgID);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_hModule = (HMODULE)hModule;
    }
    return TRUE;
}