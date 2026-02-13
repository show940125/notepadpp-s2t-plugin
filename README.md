# S2TConverter（Notepad++ 外掛） / S2TConverter (Notepad++ Plugin)

> Notepad++ 中文簡繁轉換外掛，支援「優先轉換選取內容」。
> 
> Notepad++ plugin for Chinese Simplified/Traditional conversion with selection-first behavior.

## 語言切換 / Language Navigation

- [中文說明](#中文說明)
- [English Documentation](#english-documentation)

---

## 中文說明

### 快速跳轉

- [功能特色](#功能特色)
- [建置方式](#建置方式)
- [安裝方式](#安裝方式)
- [右鍵選單（選用）](#右鍵選單選用)
- [備註](#備註)
- [跳到英文版](#english-documentation)

### 功能特色

- `簡體 -> 繁體`
- `繁體 -> 簡體`
- `自動判斷方向`（根據文字差異自動選擇轉換方向）
- 優先轉換選取內容：
  - 有選取文字時：只轉換選取內容
  - 沒有選取文字時：轉換整份文件
- 更好的編碼處理：
  - 使用目前 Scintilla 文件 code page，而不是強制 UTF-8

### 建置方式

#### 快速建置（推薦）

```bat
scripts\build_x64.bat
```

輸出檔案：

- `build\Release\S2TConverter.dll`

#### 手動建置（MSVC）

```bat
cd /d C:\Users\a0953041880\notepadpp-s2t-plugin
mkdir build\Release 2>nul
cl /nologo /LD /EHsc /std:c++17 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /DNOMINMAX src\plugin.cpp /link /NOLOGO user32.lib /DEF:S2TConverter.def /OUT:build\Release\S2TConverter.dll
```

### 安裝方式

關閉 Notepad++，將 DLL 複製到以下其中一處：

- 使用者層級（不需系統管理員）：
  - `%AppData%\Notepad++\plugins\S2TConverter\S2TConverter.dll`
- 系統層級（需要系統管理員）：
  - `C:\Program Files\Notepad++\plugins\S2TConverter\S2TConverter.dll`

之後重新開啟 Notepad++。

安裝 DLL 後：

- 外掛命令會出現在上方選單：
  - `Plugins > S2TConverter > ...`
- 右鍵選單項目為選用，需另行設定 `contextMenu.xml`。

### 右鍵選單（選用）

Notepad++ 右鍵選單由以下檔案控制：

- `%AppData%\Notepad++\contextMenu.xml`

若要在右鍵選單加入本外掛命令，請在 `<ScintillaContextMenu>` 下加入：

```xml
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Simplified -> Traditional" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Traditional -> Simplified" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Auto Detect Direction" />
```

儲存後重新啟動 Notepad++ 以套用變更。

### 備註

- 轉換引擎使用 Windows `LCMapStringEx`。
- 若追求詞彙層級（phrase-level）字典精準度，OpenCC 仍更強。

---

## English Documentation

### Quick Navigation

- [Features](#features)
- [Build](#build)
- [Install](#install)
- [Context Menu (Optional)](#context-menu-optional)
- [Notes](#notes)
- [Back to 中文說明](#中文說明)

### Features

- `Simplified -> Traditional`
- `Traditional -> Simplified`
- `Auto Detect Direction` (auto picks one direction by text difference)
- Selection-first behavior:
  - If text is selected, convert selection only
  - If nothing is selected, convert whole document
- Better encoding handling:
  - Uses current Scintilla document code page instead of forcing UTF-8

### Build

#### Quick build (recommended)

```bat
scripts\build_x64.bat
```

Output:

- `build\Release\S2TConverter.dll`

#### Manual build (MSVC)

```bat
cd /d C:\Users\a0953041880\notepadpp-s2t-plugin
mkdir build\Release 2>nul
cl /nologo /LD /EHsc /std:c++17 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /DNOMINMAX src\plugin.cpp /link /NOLOGO user32.lib /DEF:S2TConverter.def /OUT:build\Release\S2TConverter.dll
```

### Install

Close Notepad++, then copy DLL to one of these locations:

- User scope (no admin):
  - `%AppData%\Notepad++\plugins\S2TConverter\S2TConverter.dll`
- Machine scope (admin):
  - `C:\Program Files\Notepad++\plugins\S2TConverter\S2TConverter.dll`

Then reopen Notepad++.

After DLL installation:

- Plugin commands are available from the top menu:
  - `Plugins > S2TConverter > ...`
- Right-click menu entries are optional and require `contextMenu.xml` configuration.

### Context Menu (Optional)

Notepad++ right-click menu is controlled by:

- `%AppData%\Notepad++\contextMenu.xml`

Add these entries under `<ScintillaContextMenu>` if you want right-click access:

```xml
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Simplified -> Traditional" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Traditional -> Simplified" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Auto Detect Direction" />
```

Restart Notepad++ to apply context menu changes.

### Notes

- Conversion engine uses Windows `LCMapStringEx`.
- For phrase-level dictionary accuracy, OpenCC is still stronger.
