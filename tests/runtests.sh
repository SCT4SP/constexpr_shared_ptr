#!/bin/bash

#MYINCLUDE="../include/c++/14.0.1"
MYINCLUDE="../include/c++/15.0.1"

MYGCC="/opt/gcc-latest/bin/g++ -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH""
MYGCC_FLAGS="-g3 -std=c++26 -Winvalid-constexpr -fsanitize=address -static-libasan -I ${MYINCLUDE} -I ${MYINCLUDE}/x86_64-pc-linux-gnu"

echo -e "\n                        **** <<  Testing with GCC  >> ****\n"
${MYGCC} ${MYGCC_FLAGS} shared_ptr_constexpr_tests.cpp && ./a.out


# "-L /opt/gcc-latest/lib64" avoids https://github.com/votca/votca/issues/941
MYCLANG="clang++ -Wl,-rpath,"/opt/gcc-latest/lib64:$LD_LIBRARY_PATH" -L /opt/gcc-latest/lib64"
MYCLANG_FLAGS="-g3 -std=c++26 -Winvalid-constexpr -fsanitize=address -I ${MYINCLUDE} -I ${MYINCLUDE}/x86_64-pc-linux-gnu"
MYCLANG_NO_WARNINGS="-Wno-unknown-attributes -Wno-ignored-attributes -Wno-deprecated-builtins -Wno-keyword-compat -Wno-inconsistent-missing-override -Wno-user-defined-literals -Wno-unknown-warning-option -Wno-inline-namespace-reopened-noninline -Wno-implicit-exception-spec-mismatch"

echo -e "\n                        **** << Testing with Clang >> ****\n"
${MYCLANG} ${MYCLANG_NO_WARNINGS} ${MYCLANG_FLAGS} shared_ptr_constexpr_tests.cpp && ./a.out
