mkdir archive
robocopy "../../AlgNT_1/AlgNT_1/" archive "_get_cpu_time.cpp"
robocopy /s "test" "archive/test/"
robocopy . archive "AlgNT_5.cpp"
robocopy . archive "ReadMe.txt"
7z.exe a -t7z archive.7z archive