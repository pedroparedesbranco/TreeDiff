# the compiler: gcc for C program, define as g++ for C++
  CC = g++
 
  # compiler flags:
  #  -g     - this flag adds debugging information to the executable file
  #  -Wall  - this flag is used to turn on most compiler warnings
  CFLAGS  = -O3 -Wall
#   CFLAGS  = -O3
 
  # The build target 
#   TARGET = myprogram
 
#   all: $(TARGET)
 
#   $(TARGET): $(TARGET).c
#               $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
 
#   clean:
#               $(RM) $(TARGET)

all:
	$(CC) $(CFLAGS) -I./sdsl/include/ rf_nextsibling.cpp sdsl/lib/libsdsl.a -o rf_nextsibling
	$(CC) $(CFLAGS) -I./sdsl/include/ rf_postorder.cpp sdsl/lib/libsdsl.a -o rf_postorder
	$(CC) $(CFLAGS) -I./sdsl/include/ triplets.cpp sdsl/lib/libsdsl.a -o triplets_treediff
	$(CC) $(CFLAGS) -I./sdsl/include/ triplets_nlog.cpp sdsl/lib/libsdsl.a -o triplets_nlogn

clean:
	$(RM) rf_nextsibling