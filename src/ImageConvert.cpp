
#pragma comment(lib, "Shlwapi")
#pragma comment(lib, "transcodeimageuid")

#include <shobjidl_core.h>
#include <imagetranscode.h>
#include <shlwapi.h>

#include <filesystem>

#include "ImageConvert.h"

using namespace std::filesystem;


ImageConvert::ImageConvert() {
    HRESULT hr;
    hr = CoCreateInstance(CLSID_ImageTranscode, NULL, CLSCTX_INPROC_SERVER, IID_ITranscodeImage, (LPVOID*)&(this->pTransImg));
    if (hr != S_OK) throw "no Co";
}

ImageConvert::~ImageConvert() {
    if (pTransImg != NULL) pTransImg->Release();
}

HRESULT ImageConvert::convertToJpg(IShellItem* img, LPCWSTR out_f) {
    IStream* out;
    HRESULT hr;
    hr = SHCreateStreamOnFileW(out_f, STGM_CREATE | STGM_WRITE, &out);
    if (hr != S_OK) return hr;

    hr = this->pTransImg->TranscodeImage(img, 0, 0, TI_JPEG, out, NULL, NULL);
    if (hr != S_OK) {
        out->Release();
        DeleteFileW(out_f);
        return hr;
    }
    hr = out->Commit(STGC_DEFAULT);
    if (hr != S_OK) {
        out->Release();
        DeleteFileW(out_f);
        return hr;
    }
    out->Release();
    return S_OK;
}