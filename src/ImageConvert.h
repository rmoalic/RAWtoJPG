#pragma once

#include <shobjidl_core.h>
#include <imagetranscode.h>
#include <wincodec.h>

class ImageConvert {
public:
    ImageConvert();
    void convertToJpg(IShellItem* img, LPCWSTR out_f);
    void WICconvertToJpg(LPCWSTR in_f, LPCWSTR out_f);

private:
    ITranscodeImage* pTransImg = NULL;
    IWICImagingFactory* IWICFactory = NULL;
};