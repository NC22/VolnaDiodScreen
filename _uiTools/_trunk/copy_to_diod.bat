@echo off
robocopy "." "..\..\VolnaDiodScreen\_uiTools" /E /XD tmp_out_diod uiEink /XF *.exe README.md copy_to_diod.bat  minify_einkdisplay.* PREVIEW___client_eink.html PREVIEW___client_eink_uploader.html minify_diodscreen.bat
echo Копирование завершено.
pause