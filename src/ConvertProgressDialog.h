#pragma once
#include <shlobj_core.h>
#include <string>

class ConvertProgressDialog {
public:
    ConvertProgressDialog(PCWSTR title, PCWSTR sub_title, DWORD max);
    ~ConvertProgressDialog();

    void progress();
    void setMax(DWORD max);
    void setFileName(PCWSTR name);
    void reset();
    HWND getHWND();
    bool hasUserCancelled();

private:
    void updateTitle1();

    IProgressDialog* pd = NULL;
    DWORD max = 0;
    DWORD curr = 0;
    HWND hwnd;
    std::wstring sub_title;
};