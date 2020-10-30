@echo off

title Clean zabbix-agent-dbmon x86...

if exist "build" (
  echo ------------ Clean DBMON code ------------
  del /s /q /f *.o
  del /s /q /f bin\*.pdb
  del /s /q /f bin\*.exe.pdb
  del /s /q /f build\*.pdb
  del /s /q /f build\*.exe.pdb
  del /s /q /f build\*.exe.idb
  rem del /s /q /f bin\win32\*.exe
  del /s /q /f build\win32\include\messages.h
  del /s /q /f build\win32\include\messages.rc
  del /s /q /f build\win32\include\MSG00001.bin
  del /s /q /f build\win32\project\messages.h
  del /s /q /f build\win32\project\messages.rc
  del /s /q /f build\win32\project\MSG00001.bin
  del /s /q /f build\win32\project\*.res
)

echo Done
