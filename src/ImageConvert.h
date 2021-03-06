#pragma once

#include <shobjidl_core.h>
#include <imagetranscode.h>

class ImageConvert {
public:
    ImageConvert();
    ~ImageConvert();
    HRESULT convertToJpg(IShellItem* img, LPCWSTR out_f);

private:
    ITranscodeImage* pTransImg = NULL;
};