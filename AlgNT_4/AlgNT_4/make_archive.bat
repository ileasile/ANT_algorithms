mkdir archive
robocopy "../../AlgNT_1/AlgNT_1/" archive "BigInt.h"
robocopy "../../AlgNT_1/AlgNT_1/" archive "BigIntException.h"
robocopy "../../AlgNT_1/AlgNT_1/" archive "_get_cpu_time.cpp"
robocopy "../../AlgNT_3/AlgNT_3/" archive "Euclid.h"
robocopy "../../AlgNT_3/AlgNT_3/" archive "Euclid.cpp"
robocopy /s "test" "archive/test/"
robocopy . archive "main.cpp"
robocopy . archive "input.txt"
robocopy . archive "output.txt"
robocopy . archive "Readme.txt"
robocopy . archive "SquareMatrix.h"
robocopy . archive "MatrixException.h"
7z.exe a -t7z archive.7z archive