sources = \
    dispatcher.cpp \
    stl-threads-demo.cpp \
    worker.cpp

headers = \
    stl-threads-demo.hpp \
    utils/synchronized.hpp

objects = ${sources:.cpp=.o}

CXX = g++
CXXFLAGS = -O2 -Wall -Werror -std=c++17 -I utils/
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
