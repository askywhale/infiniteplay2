[_ISTool]
EnableISX=false

[Files]
Source: C:\langage\VC98\projects\infinitePlay\infinitePlay.exe; DestDir: {app}
Source: C:\langage\VC98\projects\infinitePlay\plugins\draughts.dll; DestDir: {app}\plugins
Source: C:\langage\VC98\projects\infinitePlay\plugins\awele.dll; DestDir: {app}\plugins
Source: C:\langage\VC98\projects\infinitePlay\plugins\chineseCheckers.dll; DestDir: {app}\plugins
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleurs.dll; DestDir: {app}\plugins\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\blanc.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\bleu.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\board.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\jaune.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\mask.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\miniblanc.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\miniblanc2.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\miniblancmask.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\rouge.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: C:\langage\VC98\projects\infinitePlay\plugins\5couleursdatas\vert.bmp; DestDir: {app}\plugins\5couleursdatas\
Source: plugins\aweledatas\board.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece1.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece1m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece2.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece2m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece3.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece3m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece4.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece4m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece5.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece5m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece6.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\aweledatas\piece6m.bmp; DestDir: {app}\plugins\aweledatas\
Source: plugins\draughtsdatas\board.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece1.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece1m.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece2.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece2m.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece3.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece3m.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece4.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\draughtsdatas\piece4m.bmp; DestDir: {app}\plugins\draughtsdatas\
Source: plugins\chineseCheckersdatas\board.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\hole.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\holem.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece0.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece1.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece2.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece3.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece4.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece5.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piece6.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: plugins\chineseCheckersdatas\piecem.bmp; DestDir: {app}\plugins\chineseCheckersdatas\
Source: main license.txt; DestDir: {app}

[Dirs]
Name: {app}\plugins\
Name: {app}\plugins\aweledatas
Name: {app}\plugins\5couleursdatas
Name: {app}\plugins\draughtsdatas
Name: {app}\plugins\chineseCheckersdatas

[Setup]
AppPublisher=askywhale
OutputDir=D:\net\infiniteplay
AppCopyright=Freeware, see license
AppName=InfinitePlay
AppVerName=InfinitePlay Beta 0.97
LicenseFile=C:\langage\VC98\projects\infinitePlay\main license.txt
SourceDir=C:\langage\VC98\projects\infinitePlay\
UninstallDisplayIcon={app}\infinitePlay.exe
DefaultDirName={pf}\InfinitePlay\
DefaultGroupName=InfinitePlay
OutputBaseFilename=IPlay

[UninstallDelete]
Name: {app}\prefs.ini; Type: files

[Icons]
Name: {group}\InfinitePlay; Filename: {app}\infinitePlay.exe; WorkingDir: {app}; IconFilename: {app}\infinitePlay.exe; Comment: Play InfinitePlay; IconIndex: 0
Name: {group}\License; Filename: {app}\main license.txt; WorkingDir: {app}; Comment: License of InfinitePlay
Name: {group}\Uninstall InfinitePlay; Filename: {app}\unins000.exe; WorkingDir: {app}; IconFilename: {app}\unins000.exe; Comment: Uninstall InfinitePlay

[Registry]
Root: HKCR; Subkey: .inp; ValueType: string; ValueData: InfinitePlayGameFile; Flags: uninsdeletekey
Root: HKCR; Subkey: InfinitePlayGameFile; ValueType: string; ValueData: InfinitePlay Game File; Flags: uninsdeletekey
Root: HKCR; Subkey: InfinitePlayGameFile\DefaultIcon; ValueType: string; ValueData: {app}\infinitePlay.exe,0
Root: HKCR; Subkey: InfinitePlayGameFile\Shell\open\command; ValueType: string; ValueData: """{app}\infinitePlay.exe"" ""%1"""
