CFLAGS=-Wall -O3
LDFLAGS= -L.
LDLIBS=-lboost_thread

OBJS=libeden.a
LIBOBJECTS=globals.o tinyxml.o tinyxmlerror.o tinyxmlparser.o bodypart.o organism.o groundpart.o chemicalSystem.o geneticSystemCore.o geneticSystemActions.o geneticSystemConditions.o geneticSystemCoreXMLParser.o geneticMutation.o geneticProcessor.o chemicalDistributionSystem.o geneticDummyCode.o geneticCodeFactory.o speciesDatabase.o bodypartObserver.o

LIBSRCDIR=E-Den-2014-Lib

all: libeden.a

libeden.a: $(LIBOBJECTS)
		ar rc $@ $^
		ranlib $@

globals.o: $(LIBSRCDIR)/globals.cpp
		gcc -c $^

tinyxml.o: $(LIBSRCDIR)/tinyxml.cpp
		gcc -c $^

tinyxmlerror.o: $(LIBSRCDIR)/tinyxmlerror.cpp
		gcc -c $^

tinyxmlparser.o: $(LIBSRCDIR)/tinyxmlparser.cpp
		gcc -c $^

bodypart.o: $(LIBSRCDIR)/bodypart.cpp
		gcc -c $^

organism.o: $(LIBSRCDIR)/organism.cpp
		gcc -c $^

groundpart.o: $(LIBSRCDIR)/groundpart.cpp
		gcc -c $^

chemicalSystem.o: $(LIBSRCDIR)/chemicalSystem.cpp
		gcc -c $^

geneticSystemCore.o: $(LIBSRCDIR)/geneticSystemCore.cpp
		gcc -c $^

geneticSystemActions.o: $(LIBSRCDIR)/geneticSystemActions.cpp
		gcc -c $^

geneticSystemConditions.o: $(LIBSRCDIR)/geneticSystemConditions.cpp
		gcc -c $^

geneticSystemCoreXMLParser.o: $(LIBSRCDIR)/geneticSystemCoreXMLParser.cpp
		gcc -c $^

geneticMutation.o: $(LIBSRCDIR)/geneticMutation.cpp
		gcc -c $^

geneticProcessor.o: $(LIBSRCDIR)/geneticProcessor.cpp
		gcc -c $^

chemicalDistributionSystem.o: $(LIBSRCDIR)/chemicalDistributionSystem.cpp
		gcc -c $^

geneticDummyCode.o: $(LIBSRCDIR)/geneticDummyCode.cpp
		gcc -c $^

geneticCodeFactory.o: $(LIBSRCDIR)/geneticCodeFactory.cpp
		gcc -c $^

speciesDatabase.o: $(LIBSRCDIR)/speciesDatabase.cpp
		gcc -c $^

bodypartObserver.o: $(LIBSRCDIR)/bodypartObserver.cpp
		gcc -c $^

clean:
		rm libeden.a $(LIBOBJECTS)