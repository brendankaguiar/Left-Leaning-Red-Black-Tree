# the compiler: g++ for C++ program
CXX = g++ --std=c++11

#compiler flags:
# -g adds debugging inf to executable file
# -Wall turns on most, not all, compiler warnings
CXXFLAGS = -Wall -g

# the build target executable:

Exercise5: Source.o LeftLeaningRedBlack.o   #first line lists dependency of trunk.
	$(CXX) $(CXXFLAGS) -o Exercise5 Source.o LeftLeaningRedBlack.o
#next line is building the object files with its dependencies.
Source.o: LeftLeaningRedBlack.cpp LeftLeaningRedBlack.h VoidRef.h
	$(CXX) $(CXXFLAGS) -c Source.cpp

LeftLeaningRedBlack.o: LeftLeaningRedBlack.h VoidRef.h

#indented line, known as generator line, not needed after first one bc of CXX.

clean:
	rm Exercise5 Source.o LeftLeaningRedBlack.o