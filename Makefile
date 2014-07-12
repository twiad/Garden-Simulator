LIBDIR=Libraray

LIBSRCDIR=$(LIBDIR)/E-Den-2014-Lib

LIBOBJECTS=$(LIBDIR)/globals.o $(LIBDIR)/tinyxml.o $(LIBDIR)/tinyxmlerror.o $(LIBDIR)/tinyxmlparser.o $(LIBDIR)/bodypart.o $(LIBDIR)/organism.o $(LIBDIR)/groundpart.o $(LIBDIR)/chemicalSystem.o $(LIBDIR)/geneticSystemCore.o $(LIBDIR)/geneticSystemActions.o $(LIBDIR)/geneticSystemConditions.o $(LIBDIR)/geneticSystemCoreXMLParser.o $(LIBDIR)/geneticMutation.o $(LIBDIR)/geneticProcessor.o $(LIBDIR)/chemicalDistributionSystem.o $(LIBDIR)/geneticDummyCode.o $(LIBDIR)/geneticCodeFactory.o $(LIBDIR)/speciesDatabase.o $(LIBDIR)/bodypartObserver.o

LIBOBJECTS=$(LIBDIR)/globals.o $(LIBDIR)/tinyxml.o $(LIBDIR)/tinyxmlerror.o $(LIBDIR)/tinyxmlparser.o $(LIBDIR)/bodypart.o $(LIBDIR)/organism.o $(LIBDIR)/groundpart.o $(LIBDIR)/chemicalSystem.o $(LIBDIR)/geneticSystemCore.o $(LIBDIR)/geneticSystemActions.o $(LIBDIR)/geneticSystemConditions.o $(LIBDIR)/geneticSystemCoreXMLParser.o $(LIBDIR)/geneticMutation.o $(LIBDIR)/geneticCode.o $(LIBDIR)/geneticProcessor.o $(LIBDIR)/chemicalDistributionSystem.o $(LIBDIR)/geneticDummyCode.o $(LIBDIR)/geneticCodeFactory.o $(LIBDIR)/speciesDatabase.o $(LIBDIR)/bodypartObserver.o $(LIBDIR)/runtimeManager.o


all: $(LIBDIR)/libeden.a $(LIBDIR)/libeden.so

$(LIBDIR)/libeden.a: $(LIBOBJECTS)
		ar rc $@ $^
		ranlib $@

$(LIBDIR)/libeden.so: $(LIBOBJECTS)
		g++ -Wall -shared -fPIC -o $@ $^

$(LIBDIR)/%.o: $(LIBSRCDIR)/%.cpp
		gcc -fPIC -c -o $@ $^
clean:
		rm $(LIBDIR)/libeden.a $(LIBDIR)/libeden.so $(LIBOBJECTS)