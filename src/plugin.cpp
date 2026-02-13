#include <windows.h>
#include <tchar.h>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr TCHAR kPluginName[] = TEXT("S2TConverter");
constexpr int kMenuCount = 3;
constexpr DWORD kAutoDetectFlag = 0;

// Minimal Notepad++ plugin interface declarations.
struct NppData {
    HWND _nppHandle;
    HWND _scintillaMainHandle;
    HWND _scintillaSecondHandle;
};

typedef void (*PFUNCPLUGINCMD)();

struct ShortcutKey {
    bool _isCtrl;
    bool _isAlt;
    bool _isShift;
    unsigned char _key;
};

struct FuncItem {
    TCHAR _itemName[64];
    PFUNCPLUGINCMD _pFunc;
    int _cmdID;
    bool _init2Check;
    ShortcutKey* _pShKey;
};

struct SCNotification {
    int _unused;
};

using Sci_PositionCR = intptr_t;

struct Sci_CharacterRange {
    Sci_PositionCR cpMin;
    Sci_PositionCR cpMax;
};

struct Sci_TextRange {
    Sci_CharacterRange chrg;
    char* lpstrText;
};

constexpr UINT NPPMSG = WM_USER + 1000;
constexpr UINT NPPM_GETCURRENTSCINTILLA = NPPMSG + 4;

constexpr UINT SCI_BEGINUNDOACTION = 2078;
constexpr UINT SCI_ENDUNDOACTION = 2079;
constexpr UINT SCI_GETSELECTIONSTART = 2143;
constexpr UINT SCI_GETSELECTIONEND = 2145;
constexpr UINT SCI_GETTEXTRANGE = 2162;
constexpr UINT SCI_GETTEXTLENGTH = 2183;
constexpr UINT SCI_GETCODEPAGE = 2137;
constexpr UINT SCI_SETTARGETSTART = 2190;
constexpr UINT SCI_SETTARGETEND = 2192;
constexpr UINT SCI_REPLACETARGET = 2194;

NppData g_nppData{};
FuncItem g_menuItems[kMenuCount]{};

std::wstring BytesToUtf16(const std::string& input, UINT codePage) {
    if (input.empty()) {
        return {};
    }

    const int required = MultiByteToWideChar(
        codePage, 0, input.data(), static_cast<int>(input.size()), nullptr, 0);
    if (required <= 0) {
        throw std::runtime_error("MultiByteToWideChar failed.");
    }

    std::wstring output(static_cast<size_t>(required), L'\0');
    if (MultiByteToWideChar(
            codePage, 0, input.data(), static_cast<int>(input.size()),
            output.data(), required) <= 0) {
        throw std::runtime_error("MultiByteToWideChar failed.");
    }

    return output;
}

std::string Utf16ToBytes(const std::wstring& input, UINT codePage) {
    if (input.empty()) {
        return {};
    }

    const int required = WideCharToMultiByte(
        codePage, 0, input.data(), static_cast<int>(input.size()), nullptr, 0,
        nullptr, nullptr);
    if (required <= 0) {
        throw std::runtime_error("WideCharToMultiByte failed.");
    }

    std::string output(static_cast<size_t>(required), '\0');
    if (WideCharToMultiByte(
            codePage, 0, input.data(), static_cast<int>(input.size()),
            output.data(), required, nullptr, nullptr) <= 0) {
        throw std::runtime_error("WideCharToMultiByte failed.");
    }

    return output;
}

std::wstring ConvertChineseMap(const std::wstring& input, DWORD mapFlag) {
    if (input.empty()) {
        return {};
    }

    const int required = LCMapStringEx(
        LOCALE_NAME_SYSTEM_DEFAULT, mapFlag, input.data(),
        static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr, 0);
    if (required <= 0) {
        throw std::runtime_error("LCMapStringEx failed.");
    }

    std::wstring output(static_cast<size_t>(required), L'\0');
    if (LCMapStringEx(
            LOCALE_NAME_SYSTEM_DEFAULT, mapFlag, input.data(),
            static_cast<int>(input.size()), output.data(), required, nullptr,
            nullptr, 0) <= 0) {
        throw std::runtime_error("LCMapStringEx failed.");
    }

    return output;
}

HWND CurrentScintilla() {
    int which = 0;
    SendMessage(
        g_nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0,
        reinterpret_cast<LPARAM>(&which));
    return (which == 0) ? g_nppData._scintillaMainHandle
                        : g_nppData._scintillaSecondHandle;
}

UINT GetDocumentCodePage(HWND scintilla) {
    const LRESULT cp = SendMessage(scintilla, SCI_GETCODEPAGE, 0, 0);
    return (cp > 0) ? static_cast<UINT>(cp) : CP_ACP;
}

size_t CountDifferences(const std::wstring& lhs, const std::wstring& rhs) {
    const size_t minLen = std::min(lhs.size(), rhs.size());
    size_t count = 0;
    for (size_t i = 0; i < minLen; ++i) {
        if (lhs[i] != rhs[i]) {
            ++count;
        }
    }
    count += (lhs.size() > rhs.size()) ? (lhs.size() - rhs.size())
                                       : (rhs.size() - lhs.size());
    return count;
}

std::string ReadRangeBytes(HWND scintilla, Sci_PositionCR start, Sci_PositionCR end) {
    if (end <= start) {
        return {};
    }

    const size_t byteLen = static_cast<size_t>(end - start);
    std::vector<char> buffer(byteLen + 1, '\0');
    Sci_TextRange textRange{{start, end}, buffer.data()};
    SendMessage(scintilla, SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&textRange));
    return std::string(buffer.data(), byteLen);
}

void ReplaceRangeBytes(
    HWND scintilla, Sci_PositionCR start, Sci_PositionCR end,
    const std::string& replacement) {
    SendMessage(scintilla, SCI_SETTARGETSTART, static_cast<WPARAM>(start), 0);
    SendMessage(scintilla, SCI_SETTARGETEND, static_cast<WPARAM>(end), 0);
    SendMessage(
        scintilla, SCI_REPLACETARGET, static_cast<WPARAM>(replacement.size()),
        reinterpret_cast<LPARAM>(replacement.c_str()));
}

DWORD ChooseDirection(const std::wstring& originalUtf16) {
    const std::wstring tradUtf16 =
        ConvertChineseMap(originalUtf16, LCMAP_TRADITIONAL_CHINESE);
    const std::wstring simpUtf16 =
        ConvertChineseMap(originalUtf16, LCMAP_SIMPLIFIED_CHINESE);

    const bool tradChanged = tradUtf16 != originalUtf16;
    const bool simpChanged = simpUtf16 != originalUtf16;

    if (tradChanged && !simpChanged) {
        return LCMAP_TRADITIONAL_CHINESE;
    }
    if (!tradChanged && simpChanged) {
        return LCMAP_SIMPLIFIED_CHINESE;
    }
    if (!tradChanged && !simpChanged) {
        return kAutoDetectFlag;
    }

    const size_t tradDiff = CountDifferences(originalUtf16, tradUtf16);
    const size_t simpDiff = CountDifferences(originalUtf16, simpUtf16);
    return (tradDiff >= simpDiff) ? LCMAP_TRADITIONAL_CHINESE
                                  : LCMAP_SIMPLIFIED_CHINESE;
}

void ApplyConversion(DWORD requestedMapFlag) {
    HWND scintilla = CurrentScintilla();
    if (!scintilla) {
        return;
    }

    Sci_PositionCR start = static_cast<Sci_PositionCR>(
        SendMessage(scintilla, SCI_GETSELECTIONSTART, 0, 0));
    Sci_PositionCR end = static_cast<Sci_PositionCR>(
        SendMessage(scintilla, SCI_GETSELECTIONEND, 0, 0));

    // If there is no selection, convert the entire document.
    if (start == end) {
        start = 0;
        end = static_cast<Sci_PositionCR>(
            SendMessage(scintilla, SCI_GETTEXTLENGTH, 0, 0));
    }

    const std::string originalBytes = ReadRangeBytes(scintilla, start, end);
    if (originalBytes.empty()) {
        return;
    }

    try {
        const UINT codePage = GetDocumentCodePage(scintilla);
        const std::wstring originalUtf16 = BytesToUtf16(originalBytes, codePage);
        const DWORD mapFlag = (requestedMapFlag == kAutoDetectFlag)
                                  ? ChooseDirection(originalUtf16)
                                  : requestedMapFlag;
        if (mapFlag == kAutoDetectFlag) {
            return;
        }

        const std::wstring convertedUtf16 =
            ConvertChineseMap(originalUtf16, mapFlag);
        const std::string convertedBytes = Utf16ToBytes(convertedUtf16, codePage);

        if (convertedBytes == originalBytes) {
            return;
        }

        SendMessage(scintilla, SCI_BEGINUNDOACTION, 0, 0);
        ReplaceRangeBytes(scintilla, start, end, convertedBytes);
        SendMessage(scintilla, SCI_ENDUNDOACTION, 0, 0);
    } catch (const std::exception&) {
        MessageBox(
            g_nppData._nppHandle,
            TEXT("Chinese conversion failed due to encoding/code page issues."),
            TEXT("S2TConverter"), MB_OK | MB_ICONERROR);
    }
}

void ConvertSimplifiedToTraditional() {
    ApplyConversion(LCMAP_TRADITIONAL_CHINESE);
}

void ConvertTraditionalToSimplified() {
    ApplyConversion(LCMAP_SIMPLIFIED_CHINESE);
}

void ConvertAutoDetectDirection() {
    ApplyConversion(kAutoDetectFlag);
}

void InitMenuItems() {
    lstrcpy(g_menuItems[0]._itemName, TEXT("Simplified -> Traditional"));
    g_menuItems[0]._pFunc = ConvertSimplifiedToTraditional;
    g_menuItems[0]._cmdID = 0;
    g_menuItems[0]._init2Check = false;
    g_menuItems[0]._pShKey = nullptr;

    lstrcpy(g_menuItems[1]._itemName, TEXT("Traditional -> Simplified"));
    g_menuItems[1]._pFunc = ConvertTraditionalToSimplified;
    g_menuItems[1]._cmdID = 1;
    g_menuItems[1]._init2Check = false;
    g_menuItems[1]._pShKey = nullptr;

    lstrcpy(g_menuItems[2]._itemName, TEXT("Auto Detect Direction"));
    g_menuItems[2]._pFunc = ConvertAutoDetectDirection;
    g_menuItems[2]._cmdID = 2;
    g_menuItems[2]._init2Check = false;
    g_menuItems[2]._pShKey = nullptr;
}

}  // namespace

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData) {
    g_nppData = notpadPlusData;
}

extern "C" __declspec(dllexport) const TCHAR* getName() {
    return kPluginName;
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF) {
    InitMenuItems();
    *nbF = kMenuCount;
    return g_menuItems;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode) {
    (void)notifyCode;
}

extern "C" __declspec(dllexport) LRESULT messageProc(
    UINT Message, WPARAM wParam, LPARAM lParam) {
    (void)Message;
    (void)wParam;
    (void)lParam;
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL isUnicode() {
    return TRUE;
}
