src = src
srcout = out
test = test
testout = testout

CXX = clang++
CXXFLAGS = -std=c++11 -Weverything \
-Wno-c++98-compat -Wno-c++98-compat-pedantic
LDFLAGS = -lpthread -lboost_filesystem -lboost_system

ifeq (${DEBUG_MODE},true)
CXXFLAGS += -g
else
CXXFLAGS += -O3
endif

srcobjects = $(shell find ${src} -name '*.cc' -not -name 'main.cc' | perl -pe 's|${src}/(.*)\.cc|${srcout}/$$1.o|')
testobjects = $(shell find ${test} -name '*.cc' | perl -pe 's|${test}/(.*)\.cc|${testout}/$$1.o|')

.PHONY: all test clean

all: out/main.o ${srcobjects}
	${CXX} -o vick-build $^ ${LDFLAGS}

test: ${testobjects} ${srcobjects}
	${CXX} -o ${testout}/out $^ ${LDFLAGS}
	./${testout}/out

clean:
	rm -Rf ${srcout} ${testout}
	mkdir ${srcout} ${testout}

${srcout}/main.o: ${src}/main.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

${srcout}/%.o: ${src}/%.cc ${src}/%.hh
	${CXX} ${CXXFLAGS} -o $@ -c $<

${testout}/test_main.o: ${test}/test_main.cc
	${CXX} ${CXXFLAGS} -o $@ -c $<

${testout}/%.o: ${test}/%.cc ${test}/%.hh
	${CXX} ${CXXFLAGS} -o $@ -c $<
