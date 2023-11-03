  CC = g++

  CFLAGS  = -O3 -Wall

all:
	$(CC) $(CFLAGS) -I./sdsl/include/ rf_nextsibling.cpp sdsl/lib/libsdsl.a -o rf_nextsibling
	$(CC) $(CFLAGS) -I./sdsl/include/ rf_postorder.cpp sdsl/lib/libsdsl.a -o rf_postorder
	$(CC) $(CFLAGS) -I./sdsl/include/ trip_treediff.cpp sdsl/lib/libsdsl.a -o trip_treediff
	$(CC) $(CFLAGS) -I./sdsl/include/ trip_sht.cpp sdsl/lib/libsdsl.a -o trip_sht
	$(CC) $(CFLAGS) rf_day.cpp -o rf_day

clean:
	$(RM) rf_nextsibling
	$(RM) rf_postorder
	$(RM) trip_treediff
	$(RM) trip_sht
	$(RM) rf_day