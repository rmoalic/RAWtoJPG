#pragma comment(lib, "Ole32")

#include "ConvertProgressDialog.h"
#include <sstream>

ConvertProgressDialog::ConvertProgressDialog(PCWSTR title, PCWSTR sub_title, DWORD max) {
    CoInitialize(NULL);

    HRESULT hr;
    hr = CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, reinterpret_cast<LPVOID*>(&(this->pd)));
    if (hr != S_OK) throw "no Co";

    this->max = max;
    this->sub_title = std::wstring(sub_title);
    pd->SetTitle(title);
    this->updateTitle1();

    DWORD flags = PROGDLG_AUTOTIME;
    pd->StartProgressDialog(NULL, NULL, flags, NULL);
    pd->SetProgress(0, this->max);

    IOleWindow* olewin;
    pd->QueryInterface(IID_PPV_ARGS(&olewin));
    olewin->GetWindow(&hwnd);
    olewin->Release();

    while (! IsWindowVisible(hwnd)) { Sleep(100); }
}

void ConvertProgressDialog::progress() {
    this->curr++;

    pd->SetProgress(this->curr, this->max);
    this->updateTitle1();

    if (this->curr >= this->max) {
        this->reset();
    }
}

void ConvertProgressDialog::updateTitle1() {
    std::wstringstream stream;
    stream << this->sub_title << L" (" << this->curr << L"/" << this->max << L")";
    pd->SetLine(1, stream.str().c_str(), false, NULL);
}

void ConvertProgressDialog::setFileName(PCWSTR name) {
    pd->SetLine(2, name, false, NULL);
}

void ConvertProgressDialog::reset() {
    this->curr = 0;
    pd->SetLine(1, L"", false, NULL);
    pd->SetLine(2, L"", false, NULL);
    pd->StopProgressDialog();
}

void ConvertProgressDialog::setMax(DWORD val) {
    this->max = val;
}
bool ConvertProgressDialog::hasUserCancelled() {
    return pd->HasUserCancelled();
}

HWND ConvertProgressDialog::getHWND() {
    return this->hwnd;
}