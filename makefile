LIBS=`root-config --libs`
CFLAGS=`root-config --cflags`
# set compiler options:
#   -g  = debugging
#   -O# = optimisation

output: jsonlib.o region.o Expdata.o Histogram.o main.o
	g++ -std=c++11 -g main.o jsonlib.o region.o Expdata.o Histogram.o -o output $(LIBS) $(CFLAGS)
jsonlib.o: jsonlib.cpp jsonlib.h
	g++ -std=c++11 -g -c jsonlib.cpp
region.o: region.cpp  region.h
	g++ -std=c++11 -g -c region.cpp
Expdata.o: Expdata.cpp Expdata.h
	g++ -std=c++11 -g -c Expdata.cpp
Histogram.o: Histogram.cpp Histogram.h
	g++ -std=c++11 -g -c Histogram.cpp
main.o: main.cpp        
	g++ -std=c++11 -g -c main.cpp $(LIBS) $(CFLAGS)
clean:
	rm jsonlib.o region.o Expdata.o Histogram.o main.o