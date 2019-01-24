COMPILER_CPP=clang++
COMPILER_C=clang


./object/serial_test:./object/upre_serial_lib.o ./object/serial_test.o
	${COMPILER_CPP} ./object/upre_serial_lib.o ./object/serial_test.o -o ./object/serial_test
./object/upre_serial_lib.o:./code/upre_serial_lib.cpp
	${COMPILER_CPP} ./code/upre_serial_lib.cpp -c -o ./object/upre_serial_lib.o -O2
./object/serial_test.o:./code/serial_test.cpp
	${COMPILER_CPP} ./code/serial_test.cpp -c -o ./object/serial_test.o -O2

