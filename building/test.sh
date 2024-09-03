cd ..
cat test/testPreset.hpp > debugMode.hpp
rm -f test/JNet
rm -f build/JNet
cmake --build build
mkdir test
cp build/JNet test/JNet
rm -f build/JNet
./test/JNet