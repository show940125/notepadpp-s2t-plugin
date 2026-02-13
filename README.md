# S2TConverter (Notepad++ Plugin)

Notepad++ plugin for Chinese conversion with selection-first behavior.

## Features

- `Simplified -> Traditional`
- `Traditional -> Simplified`
- `Auto Detect Direction` (auto picks one direction by text difference)
- Selection first:
  - If text is selected, convert selection only
  - If nothing is selected, convert whole document
- Better encoding handling:
  - Uses current Scintilla document code page instead of forcing UTF-8

## Build

## Quick build (recommended)

```bat
scripts\build_x64.bat
```

Output:

- `build\Release\S2TConverter.dll`

## Manual build (MSVC)

```bat
cd /d C:\Users\a0953041880\notepadpp-s2t-plugin
mkdir build\Release 2>nul
cl /nologo /LD /EHsc /std:c++17 /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN /DNOMINMAX src\plugin.cpp /link /NOLOGO user32.lib /DEF:S2TConverter.def /OUT:build\Release\S2TConverter.dll
```

## Install

Close Notepad++, then copy DLL to one of these:

- User scope (no admin):
  - `%AppData%\Notepad++\plugins\S2TConverter\S2TConverter.dll`
- Machine scope (admin):
  - `C:\Program Files\Notepad++\plugins\S2TConverter\S2TConverter.dll`

Then reopen Notepad++.

After DLL installation:

- Plugin commands are always available from top menu:
  - `Plugins > S2TConverter > ...`
- Right-click menu entries are optional and require `contextMenu.xml` configuration.

## Context Menu

This part is optional.

Notepad++ right-click menu is controlled by:

- `%AppData%\Notepad++\contextMenu.xml`

Add these entries under `<ScintillaContextMenu>` if you want right-click access:

```xml
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Simplified -> Traditional" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Traditional -> Simplified" />
<Item FolderName="Plugin commands" TranslateID="contextMenu-PluginCommands" PluginEntryName="S2TConverter" PluginCommandItemName="Auto Detect Direction" />
```

Then restart Notepad++ to apply context menu changes.

## Notes

- Conversion engine uses Windows `LCMapStringEx`.
- For phrase-level dictionary accuracy, OpenCC is still stronger.
