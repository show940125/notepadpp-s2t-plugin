# S2TConverter (Notepad++ Plugin)

這是一個最小化 Notepad++ 外掛，只有一個功能：

- 把簡體中文轉成繁體中文

轉換方式使用 Windows `LCMapStringEx(..., LCMAP_TRADITIONAL_CHINESE, ...)`，不需要外部字典套件。

## 功能行為

- 有選取文字：只轉換選取範圍
- 沒有選取文字：轉換整份文件

## 建置需求

- Windows
- Visual Studio 2022 (含 C++ Desktop workload)
- CMake 3.20+

## 建置步驟 (x64)

```powershell
cd C:\Users\a0953041880\notepadpp-s2t-plugin
cmake -S . -B build -A x64
cmake --build build --config Release
```

輸出檔案：

- `C:\Users\a0953041880\notepadpp-s2t-plugin\build\Release\S2TConverter.dll`

## 安裝到 Notepad++

1. 關閉 Notepad++
2. 建立資料夾：
   - `%AppData%\Notepad++\plugins\S2TConverter\`
3. 把 `S2TConverter.dll` 複製到該資料夾
4. 重新開啟 Notepad++
5. 到選單 `Plugins > S2TConverter > Convert Simplified -> Traditional`

## 注意

- 建議使用 Notepad++ x64 + x64 外掛；如果你是 32 位元 Notepad++，請改用 `-A Win32` 重新編譯。
- Windows 內建映射不是詞庫等級的語意轉換，少數用詞可能不如 OpenCC 精確。
