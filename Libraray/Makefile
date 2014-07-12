LIBOBJECTS=globals.o tinyxml.o tinyxmlerror.o tinyxmlparser.o bodypart.o organism.o groundpart.o chemicalSystem.o geneticSystemCore.o geneticSystemActions.o geneticSystemConditions.o geneticSystemCoreXMLParser.o geneticMutation.o geneticProcessor.o chemicalDistributionSystem.o geneticDummyCode.o geneticCodeFactory.o speciesDatabase.o bodypartObserver.o

LIBSRCDIR=E-Den-2014-Lib

all: libeden.a libeden.so

libeden.a: $(LIBOBJECTS)
		ar rc $@ $^
		ranlib $@

libeden.so: $(LIBOBJECTS)
		g++ -Wall -shared -fPIC -o $@ $^

%.o: $(LIBSRCDIR)/%.cpp
		gcc -fPIC -c $^
clean:
		rm libeden.a libeden.so $(LIBOBJECTS)