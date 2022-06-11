
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

    hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&IWICFactory)
    );
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

void ImageConvert::WICconvertToJpg(LPCWSTR in_f, LPCWSTR out_f) {
    IStream* out;
    HRESULT hr;

    // Create a decoder
    IWICBitmapDecoder* pDecoder = NULL;

    hr = IWICFactory->CreateDecoderFromFilename(
        in_f,                            // Image to be decoded
        NULL,                            // Do not prefer a particular vendor
        GENERIC_READ,                    // Desired read access to the file
        WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
        &pDecoder                        // Pointer to the decoder
    );
    if (hr != S_OK) throw "no Decode";

    IWICBitmapFrameDecode* pFrameDecode = NULL;
    hr = pDecoder->GetFrame(0, &pFrameDecode);
    if (hr != S_OK) throw "no Decode";

    IWICBitmapSource* pThumbnail = NULL;
    hr = pFrameDecode->GetThumbnail(&pThumbnail);
    if (hr != S_OK) pThumbnail = NULL;

    IWICMetadataQueryReader* pQueryReader = NULL;
    hr = pFrameDecode->GetMetadataQueryReader(&pQueryReader);
    if (hr != S_OK) pQueryReader = NULL;

    IWICMetadataQueryWriter* pQueryWriter = NULL;
    if (pQueryReader != NULL) {
        hr = IWICFactory->CreateQueryWriterFromReader(pQueryReader, NULL, &pQueryWriter);
        if (hr != S_OK) pQueryWriter = NULL;
    }

    //encode

    IWICBitmapEncoder* pEncoder = NULL;
    hr = IWICFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &pEncoder);
    if (hr != S_OK) throw "no Encode";

    hr = SHCreateStreamOnFileW(out_f, STGM_CREATE | STGM_WRITE, &out);
    if (hr != S_OK) throw "no File";

    hr = pEncoder->Initialize(out, WICBitmapEncoderNoCache);
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }

    IWICBitmapFrameEncode* pFrameEncode = NULL;
    IPropertyBag2* pPropEncode = NULL;

    hr = pEncoder->CreateNewFrame(&pFrameEncode, &pPropEncode);
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }
    hr = pFrameEncode->Initialize(pPropEncode);
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }
    if (pThumbnail != NULL) {
        hr = pFrameEncode->SetThumbnail(pThumbnail);
        if (hr != S_OK) {
            out->Release();
            throw "no Transcode";
        }
    }
    if (pQueryWriter != NULL) {
        IWICMetadataQueryWriter* pMetaDataWriter;

        hr = pFrameEncode->GetMetadataQueryWriter(&pMetaDataWriter);
        if (hr != S_OK) {
            out->Release();
            throw "no Transcode";
        }
        PROPVARIANT value;
        PropVariantInit(&value);
        value.vt = VT_UNKNOWN;
        value.punkVal = pQueryWriter;
        value.punkVal->AddRef();
        hr = pMetaDataWriter->SetMetadataByName(L"/", &value);
    }

    hr = pFrameEncode->WriteSource(pFrameDecode, NULL);
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }
    hr = pFrameEncode->Commit();
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }
    hr = pEncoder->Commit();
    if (hr != S_OK) {
        out->Release();
        throw "no Transcode";
    }

    out->Commit(STGC_DEFAULT);
    out->Release();
}

