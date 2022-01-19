export AFL_SKIP_CPUFREQ=1
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

git clone https://github.com/AFLplusplus/AFLplusplus.git
cd AFLplusplus
make
cd ..
mkdir seeds
echo input > seeds/inp1 
AFLplusplus/afl-gcc -o fuzz_mjyint test_mjyint.c
AFLplusplus/afl-fuzz -i seeds -o out ./fuzz_mjyint
