
#pragma comment(lib, "Shlwapi")
#pragma comment(lib, "transcodeimageuid")

#include <shobjidl_core.h>
#include <imagetranscode.h>
#include <shlwapi.h>

#include <filesystem>

#include "ImageConvert.h"

using namespace std::filesystem;


ImageConvert::ImageConvert() {
    CoInitialize(NULL);

    HRESULT hr;
    hr = CoCreateInstance(CLSID_ImageTranscode, NULL, CLSCTX_INPROC_SERVER, IID_ITranscodeImage, (LPVOID*)&(this->pTransImg));
    if (hr != S_OK) throw "no Co";
}

void ImageConvert::convertToJpg(IShellItem* img, LPCWSTR out_f) {
    IStream* out;
    HRESULT hr;
    hr = SHCreateStreamOnFileW(out_f, STGM_CREATE | STGM_WRITE, &out);
    if (hr != S_OK) throw "no File";

    hr = this->pTransImg->TranscodeImage(img, 0, 0, TI_JPEG, out, NULL, NULL);
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }
    out->Commit(STGC_DEFAULT);
    out->Release();
}