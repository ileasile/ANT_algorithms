REM replace include directory with yours, and place libNTL.a in the same directory.
g++ -std=c++11 main.cpp -I "../../AlgNT_5/NTL/include" -static -L . -lNTL -o alg_nt7.exe
alg_nt7.exe
