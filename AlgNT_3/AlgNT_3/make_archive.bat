mkdir archive
robocopy "../../AlgNT_1/AlgNT_1/" archive "BigInt.h"
robocopy "../../AlgNT_1/AlgNT_1/" archive "BigIntException.h"
robocopy "../../AlgNT_1/AlgNT_1/" archive "_get_cpu_time.cpp"
robocopy /s "tests" "archive/tests/" gcd* lcm* make*
robocopy . archive "Euclid.cpp"
robocopy . archive "Euclid.h"
robocopy . archive "main.cpp"
robocopy . archive "Readme.txt"
7z.exe a -t7z archive.7z archive