src = src
srcout = out
test = test
testout = testout
bench = bench
benchout = benchout

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wpedantic -Wextra
LDFLAGS = -lpthread -lboost_filesystem -lboost_system

ifeq (${DEBUG_MODE},true)
CXXFLAGS += -g
else
CXXFLAGS += -O3
endif

srcobjects = $(shell find ${src} -name '*.cc' -not -name 'main.cc' | perl -pe 's|${src}/(.*)\.cc|${srcout}/$$1.o|')
testobjects = $(shell find ${test} -name '*.cc' | perl -pe 's|${test}/(.*)\.cc|${testout}/$$1.o|')
benchobjects = $(shell find ${bench} -name '*.cc' | perl -pe 's|${bench}/(.*)\.cc|${benchout}/$$1.o|')

.PHONY: all test bench clean

all: out/main.o ${srcobjects}
	${CXX} -o vick-build $^ ${LDFLAGS}

test: ${testobjects} ${srcobjects}
	${CXX} -o ${testout}/out $^ ${LDFLAGS}
	./${testout}/out

bench: ${benchobjects} ${srcobjects}
	${CXX} -o ${benchout}/out $^ ${LDFLAGS}
	./${benchout}/out

clean:
	rm -Rf ${srcout} ${testout} ${benchout}
	mkdir ${srcout} ${testout} ${benchout}

${srcout}/main.o: ${src}/main.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

${srcout}/%.o: ${src}/%.cc ${src}/%.hh
	${CXX} ${CXXFLAGS} -o $@ -c $<

${testout}/test_main.o: ${test}/test_main.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

${testout}/%.o: ${test}/%.cc ${test}/%.hh
	${CXX} ${CXXFLAGS} -o $@ -c $<

${benchout}/bench_main.o: ${bench}/bench_main.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

${benchout}/%.o: ${bench}/%.cc ${bench}/%.hh
	${CXX} ${CXXFLAGS} -o $@ -c $<
