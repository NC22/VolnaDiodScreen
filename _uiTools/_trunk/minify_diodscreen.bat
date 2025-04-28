@echo off
pushd "%~1"

SET outdir=.\tmp_out_diod\
SET srcdir=
::SET mergedcl=mergedclient.html

::break>%mergedcl%
::@echo.>> %mergedcl%
::copy %mergedcl% + %srcdir%ui\client.html %mergedcl%
::@echo.>> %mergedcl%

:: device related enviroment

::copy %mergedcl% + %srcdir%uiDiodScreen\init.js %mergedcl%
::@echo.>> %mergedcl%

htmlpack.exe %srcdir%uiDiodScreen\ENV.js %outdir%ui\out\env 0
htmlpack.exe %srcdir%uiDiodScreen\init.js %outdir%ui\out\init 0
htmlpack.exe %srcdir%uiDiodScreen\initUpload.js %outdir%ui\out\initUpload 0
htmlpack.exe %srcdir%uiDiodScreen\KellyUploader.js %outdir%ui\out\KellyUploader 0
htmlpack.exe %srcdir%uiDiodScreen\KellyUIFileManager.js %outdir%ui\out\KellyUIFileManager 0
htmlpack.exe %srcdir%uiDiodScreen\KellyUploader.js %outdir%ui\out\KellyUploader 0
htmlpack.exe %srcdir%uiDiodScreen\KellyUploader.tools.js %outdir%ui\out\KellyUploader_tools 0
htmlpack.exe %srcdir%uiDiodScreen\client_uploader.html %outdir%ui\out\client_uploader 0
htmlpack.exe %srcdir%uiDiodScreen\locale\localeEN.js %outdir%ui\out\locale_en\locale 0
htmlpack.exe %srcdir%uiDiodScreen\locale\localeRU.js %outdir%ui\out\locale_ru\locale 0
htmlpack.exe %srcdir%uiDiodScreen\style.uploader.css %outdir%ui\out\style_uploader 0

:: common

htmlpack.exe %srcdir%ui\client.html %outdir%ui\out\client 0
htmlpack.exe %srcdir%ui\client_end.html %outdir%ui\out\client_end 0
htmlpack.exe %srcdir%ui\KellyEnv.js %outdir%ui\out\KellyEnv 0
htmlpack.exe %srcdir%ui\KellyTools.js %outdir%ui\out\KellyTools 0
htmlpack.exe %srcdir%ui\KellyTools.select.widget.js %outdir%ui\out\KellyTools_select_widget 0
htmlpack.exe %srcdir%ui\KellyTools.list.widget.js %outdir%ui\out\KellyTools_list_widget 0
htmlpack.exe %srcdir%ui\style.css %outdir%ui\out\style 0

::copy %srcdir%lib\WebServer.cpp %outdir%
::copy %srcdir%lib\WebServer.h %outdir%

xcopy ".\tmp_out_diod\*" "..\..\VolnaDiodScreen\DiodScreen\src\" /E /H /Y /C

pause