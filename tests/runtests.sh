#!/bin/bash

MYINCLUDE="../include/c++/14.0.1"

MYGCC="/opt/gcc-latest/bin/g++ -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH""
MYGCCFLAGS="-g -std=c++26 -Winvalid-constexpr -fsanitize=address -static-libasan -I ${MYINCLUDE} -I ${MYINCLUDE}/x86_64-pc-linux-gnu"

echo "Testing with GCC"
${MYGCC} ${MYGCCFLAGS} shared_ptr_constexpr_tests.cpp && ./a.out


# "-L /opt/gcc-latest/lib64" avoids https://github.com/votca/votca/issues/941
MYCLANG="clang++-17 -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH" -L /opt/gcc-latest/lib64"
MYCLANGFLAGS="-g -std=c++26 -Winvalid-constexpr -fsanitize=address -I ${MYINCLUDE} -I ${MYINCLUDE}/x86_64-pc-linux-gnu"

echo "Testing with Clang"
${MYCLANG} ${MYCLANGFLAGS} shared_ptr_constexpr_tests.cpp && ./a.out
