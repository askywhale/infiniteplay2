@echo off
echo %1
cd %1

copy ..\Awele\bin\Release\*.dll bin\Release\games
copy ..\ChineseCheckers\bin\Release\*.dll bin\Release\games
copy ..\Draughts\bin\Release\*.dll bin\Release\games
copy ..\FiveColors\bin\Release\*.dll bin\Release\games
copy ..\MeepleBattle\bin\Release\*.dll bin\Release\games
copy ..\Planetoid\bin\Release\*.dll bin\Release\games

copy ..\Awele\bin\Debug\*.dll bin\Debug\games
copy ..\ChineseCheckers\bin\Debug\*.dll bin\Debug\games
copy ..\Draughts\bin\Debug\*.dll bin\Debug\games
copy ..\FiveColors\bin\Debug\*.dll bin\Debug\games
copy ..\MeepleBattle\bin\Debug\*.dll bin\Debug\games
copy ..\Planetoid\bin\Debug\*.dll bin\Debug\games

copy bin\Release\InfinitePlay.exe published\
rem copy bin\Release\games\*.dll published\games
copy bin\Debug\games\*.dll published\games

echo ***** "Published" created *****
pause