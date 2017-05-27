; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=InfinitePlay SDK
AppVerName=InfinitePlay SDK V1
AppPublisher=Askywhale
AppPublisherURL=infiniteplay.shim.net
AppSupportURL=infiniteplay.shim.net
AppUpdatesURL=infiniteplay.shim.net
DefaultDirName={pf}\InfinitePlay
DefaultGroupName=InfinitePlay
LicenseFile=C:\langage\VC98\projects\infinitePlay\main license.txt
InfoBeforeFile=C:\langage\VC98\projects\infinitePlay\sdk.txt
OutputBaseFilename=IPlaySdk
OutputDir=D:\net\infinitePlay
Uninstallable=true
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Files]
Source: C:\langage\VC98\projects\infinitePlay\5couleurs\*.*; DestDir: {app}\5couleurs; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\awele\*.*; DestDir: {app}\awele; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\chineseCheckers\*.*; DestDir: {app}\chineseCheckers; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\draughts\*.*; DestDir: {app}\draughts; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\resource.h; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\binaryGame.h; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\main license.txt; DestDir: {app}; CopyMode: alwaysoverwrite
Source: C:\langage\VC98\projects\infinitePlay\default.cpp; DestDir: {app}; CopyMode: alwaysoverwrite

[Icons]

[_ISTool]
EnableISX=false
