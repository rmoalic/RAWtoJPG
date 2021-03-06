
#include "ConvertProgressDialog.h"
#include "ImageConvert.h"
#include "ressource.h"
#include "comdef.h"
#include <shobjidl_core.h>

#include <filesystem>
#include <thread>
#include <sstream>

using namespace std::filesystem;

extern long g_cComponents;
extern HMODULE g_hModule;

bool FileExists(LPCWSTR fileName) {
    DWORD fileAttr = GetFileAttributesW(fileName);
    if (0xFFFFFFFF == fileAttr && GetLastError() == ERROR_FILE_NOT_FOUND) return false;
    return true;
}

class ConvertCommand : public IExecuteCommand, public IObjectWithSelection, public IShellExtInit {
public:
    static bool AskFileReplace(HWND parent, LPCWSTR filename) {
        WCHAR dialog_title[50];
        LoadStringW(g_hModule, IDS_CONVERTCOMMAND_ASK_REPLACE_TITLE, (LPWSTR)&dialog_title, 49);
        WCHAR dialog_text[50];
        LoadStringW(g_hModule, IDS_CONVERTCOMMAND_ASK_REPLACE_TEXT, (LPWSTR)&dialog_text, 49);

        int msgboxID = MessageBoxW(parent, (std::wstring(filename) + dialog_text).c_str(),
            dialog_title, MB_ICONEXCLAMATION | MB_YESNO);

        return msgboxID == IDYES;
    }

    static bool ErrorDialog(HWND parent, HRESULT last_error) {
        WCHAR dialog_title[50];
        LoadStringW(g_hModule, IDS_CONTEXT_MENU, (LPWSTR)&dialog_title, 49);
        WCHAR dialog_text[100];
        LoadStringW(g_hModule, IDS_CONVERTCOMMAND_ERROR_TEXT, (LPWSTR)&dialog_text, 99);

        _com_error err(last_error);

        std::wstringstream stream;
        stream << dialog_text << L"\n\ncode: 0x" << std::hex << last_error << L" (" << err.ErrorMessage() << L")";

        int msgboxID = MessageBoxW(parent, stream.str().c_str(),
            dialog_title, MB_SETFOREGROUND | MB_ICONERROR | MB_OK);

        return msgboxID == IDYES;
    }

    static void executeConvert(ConvertCommand* cc) {
        DWORD select_count;
        cc->m_sia->GetCount(&select_count);

        WCHAR dialog_title[50];
        LoadStringW(g_hModule, IDS_CONTEXT_MENU, (LPWSTR)&dialog_title, 49);
        WCHAR dialog_text[50];
        LoadStringW(g_hModule, IDS_CONVERTCOMMAND_PROGRESSDIALOG_TEXT, (LPWSTR)&dialog_text, 49);

        ConvertProgressDialog dialog = ConvertProgressDialog(dialog_title, dialog_text, select_count);
        HWND dialog_id = dialog.getHWND();
        ImageConvert ic = ImageConvert();
        HRESULT last_error_hr = S_OK;

        for (int i = 0; i < select_count; i++) {
            IShellItem* it;
            cc->m_sia->GetItemAt(i, &it);

            LPWSTR sfile_name;
            it->GetDisplayName(SIGDN_FILESYSPATH, &sfile_name);
            path p = path(sfile_name);
            dialog.setFileName(p.filename().c_str());

            p.replace_extension(".jpg");

            std::wstring out_file = std::wstring(p.wstring());
            LPCWSTR out_f = out_file.c_str();

            HRESULT convert_hr = S_OK;
            if (lstrcmpW(sfile_name, out_f) != 0) {
                if (FileExists(out_f)) {
                    if (cc->replace_all || AskFileReplace(dialog_id, out_f)) {
                        convert_hr = ic.convertToJpg(it, out_f);
                    }
                }
                else {
                    convert_hr = ic.convertToJpg(it, out_f);
                }
            }
            if (convert_hr != S_OK) {
                last_error_hr = convert_hr;
            }

            dialog.progress();
            CoTaskMemFree(sfile_name);
            if (dialog.hasUserCancelled()) {
                dialog.close();
                break;
            };
        }

        dialog.reset();
        if (last_error_hr != S_OK) {
            ErrorDialog(dialog_id, last_error_hr);
        }
        dialog.close();

        cc->Release();
    }

    // IShellExtInit
    HRESULT __stdcall ConvertCommand::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj,
        HKEY hkeyProgID) {
        return S_OK;
    }

    // IExecuteCommand
    HRESULT __stdcall ConvertCommand::Execute(void) {
        this->AddRef();
        std::thread convert_thread(&executeConvert, this);
        convert_thread.detach();
        return S_OK;
    }

    HRESULT __stdcall ConvertCommand::SetKeyState(DWORD keystate) {
        this->replace_all = keystate & MK_CONTROL;
        return S_OK;
    }
    HRESULT __stdcall ConvertCommand::SetParameters(LPCWSTR parameters) { return S_OK; }
    HRESULT __stdcall ConvertCommand::SetPosition(POINT point) { return S_OK; }
    HRESULT __stdcall ConvertCommand::SetShowWindow(int show) { return S_OK; }
    HRESULT __stdcall ConvertCommand::SetNoShowUI(BOOL no_ui) { return S_OK; }
    HRESULT __stdcall ConvertCommand::SetDirectory(LPCWSTR dir) { return S_OK; }

    // IObjectWithSelection
    HRESULT __stdcall ConvertCommand::SetSelection(IShellItemArray* items) {
        if (items) items->AddRef();
        if (this->m_sia) this->m_sia->Release();
        this->m_sia = items;
        return S_OK;
    }
    HRESULT __stdcall ConvertCommand::GetSelection(const IID& iid, void** selection) {
        if (this->m_sia) return this->m_sia->QueryInterface(iid, selection);
        *selection = NULL;
        return E_NOINTERFACE;
    }

    // IUnknown
    HRESULT __stdcall ConvertCommand::QueryInterface(const IID& iid, void** ppv) {
        if (iid == IID_IUnknown) {
            *ppv = this;
        }
        else if (iid == IID_IExecuteCommand) {
            *ppv = static_cast<IExecuteCommand*>(this);
        }
        else if (iid == IID_IObjectWithSelection) {
            *ppv = static_cast<IObjectWithSelection*>(this);
        }
        else if (iid == IID_IShellExtInit) {
            *ppv = static_cast<IShellExtInit*>(this);
        }
        else {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    ULONG __stdcall ConvertCommand::AddRef() { return InterlockedIncrement(&m_cRef); }

    ULONG __stdcall ConvertCommand::Release() {
        if (InterlockedDecrement(&m_cRef) == 0) {
            delete this;
            return 0;
        }
        return m_cRef;
    }

    ConvertCommand() : m_cRef(1) { InterlockedIncrement(&g_cComponents); }

    ~ConvertCommand() {
        if (this->m_sia) this->m_sia->Release();
        InterlockedDecrement(&g_cComponents);
    }

private:
    long m_cRef;
    IShellItemArray* m_sia = NULL;
    bool replace_all = false;
};
