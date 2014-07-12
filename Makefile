GWENDIR=../../gwen/GWEN/gwen

LIBDIR=Libraray
MAINSRCDIR=E-Den-2014-Solution/E-Den-2014-Testbench

LIBSRCDIR=$(LIBDIR)/E-Den-2014-Lib

LIBOBJECTS=$(LIBDIR)/globals.o $(LIBDIR)/tinyxml.o $(LIBDIR)/tinyxmlerror.o $(LIBDIR)/tinyxmlparser.o $(LIBDIR)/bodypart.o $(LIBDIR)/organism.o $(LIBDIR)/groundpart.o $(LIBDIR)/chemicalSystem.o $(LIBDIR)/geneticSystemCore.o $(LIBDIR)/geneticSystemActions.o $(LIBDIR)/geneticSystemConditions.o $(LIBDIR)/geneticSystemCoreXMLParser.o $(LIBDIR)/geneticMutation.o $(LIBDIR)/geneticCode.o $(LIBDIR)/geneticProcessor.o $(LIBDIR)/chemicalDistributionSystem.o $(LIBDIR)/geneticDummyCode.o $(LIBDIR)/geneticCodeFactory.o $(LIBDIR)/speciesDatabase.o $(LIBDIR)/bodypartObserver.o $(LIBDIR)/runtimeManager.o

MAINOBJECTS=SDLGwenStatusWindow.o SDLGwenStatisticsWindow.o SDLGwenOrgnismInspector.o organismPrinter.o SDL_WindowGroundpart.o E-Den-Testbench.o

all: $(LIBDIR)/libeden.a $(LIBDIR)/libeden.so E-Den-2014

$(LIBDIR)/libeden.a: $(LIBOBJECTS)
		ar rc $@ $^
		ranlib $@

$(LIBDIR)/libeden.so: $(LIBOBJECTS)
		g++ -Wall -shared -fPIC -o $@ $^

$(LIBDIR)/%.o: $(LIBSRCDIR)/%.cpp
		gcc -fPIC -c -o $@ $^

MAINDEPS=-I$(GWENDIR)/include -L$(GWENDIR)/lib/ -llibgwen_static.a -I$(LIBSRCDIR) -L$(LIBDIR) -llibeden.a

E-Den-2014: $(MAINOBJECTS)
		g++ -Wall $(MAINDEPS) -o $@ $^

%.o: $(MAINSRCDIR)/%.cpp
		gcc -fPIC $(MAINDEPS) -c -o $@ $^

clean:
		rm $(LIBDIR)/libeden.a $(LIBDIR)/libeden.so $(LIBOBJECTS)