sources = \
    dispatcher.cpp \
    stl-threads-demo.cpp \
    worker.cpp

headers = stl-threads-demo.hpp
objects = ${sources:.cpp=.o}

CXX = g++
CXXFLAGS = -O2 -Wall -Werror
LDFLAGS = -pthread

binary=stl-threads-demo

.PHONY: clean all
all: $(binary)


$(binary): $(objects)
	$(CXX) -o $@ $? $(LDFLAGS)


.cpp.o: $(headers)
#	The following implicit rule is applied automatically
#	$(CXX) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(binary) $(objects)
