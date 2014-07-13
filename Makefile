GWENDIR=gwen
GWENLIBARCH=linux/gmake

LIBDIR=Libraray
MAINSRCDIR=E-Den-2014-Solution/E-Den-2014-Testbench

LIBSRCDIR=$(LIBDIR)/E-Den-2014-Lib

GCCFLAGS=-fPIC -ffast-math -O2

LIBOBJECTS=$(LIBDIR)/globals.o $(LIBDIR)/tinyxml.o $(LIBDIR)/tinyxmlerror.o $(LIBDIR)/tinyxmlparser.o $(LIBDIR)/bodypart.o $(LIBDIR)/organism.o $(LIBDIR)/groundpart.o $(LIBDIR)/chemicalSystem.o $(LIBDIR)/geneticSystemCore.o $(LIBDIR)/geneticSystemActions.o $(LIBDIR)/geneticSystemConditions.o $(LIBDIR)/geneticSystemCoreXMLParser.o $(LIBDIR)/geneticMutation.o $(LIBDIR)/geneticCode.o $(LIBDIR)/geneticProcessor.o $(LIBDIR)/chemicalDistributionSystem.o $(LIBDIR)/geneticDummyCode.o $(LIBDIR)/geneticCodeFactory.o $(LIBDIR)/speciesDatabase.o $(LIBDIR)/bodypartObserver.o $(LIBDIR)/runtimeManager.o

MAINOBJECTS=SDLGwenStatusWindow.o SDLGwenStatisticsWindow.o SDLGwenOrgnismInspector.o organismPrinter.o SDL_WindowGroundpart.o E-Den-Testbench.o

all: $(GWENDIR)/lib/$(GWENLIBARCH)/libGWEN-Renderer-SDL2.a $(GWENDIR)/lib/$(GWENLIBARCH)/libgwen_static.a $(LIBDIR)/libeden.a E-Den-2014

$(LIBDIR)/libeden.a: $(LIBOBJECTS)
		ar rc $@ $^
		ranlib $@

$(LIBDIR)/libeden.so: $(LIBOBJECTS)
		g++ -Wall -shared -fPIC -o $@ $^

$(LIBDIR)/%.o: $(LIBSRCDIR)/%.cpp
		gcc $(GCCFLAGS) -c -o $@ $^

MAINDEPS=-I$(GWENDIR)/include -L$(GWENDIR)/lib/$(GWENLIBARCH)/ -lgwen_static -lGWEN-Renderer-SDL2 -I$(LIBSRCDIR) -L$(LIBDIR) -leden -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lboost_thread-mt -lboost_system-mt

E-Den-2014: $(MAINOBJECTS)
		g++ $^ $(MAINDEPS) -o $@

%.o: $(MAINSRCDIR)/%.cpp
		gcc $(GCCFLAGS) $(MAINDEPS) -c -o $@ $^

$(GWENDIR)/lib/$(GWENLIBARCH)/libgwen_static.a:
		@cd $(GWENDIR)/Projects/$(GWENLIBARCH) && ${MAKE} --no-print-directory -C . -f GWEN-Static.make

$(GWENDIR)/lib/$(GWENLIBARCH)/libGWEN-Renderer-SDL2.a:
		@cd $(GWENDIR)/Projects/$(GWENLIBARCH) && ${MAKE} --no-print-directory -C . -f Renderer-SDL2.make

clean:
		(rm E-Den-2014 $(LIBDIR)/libeden.a $(LIBDIR)/libeden.so $(LIBOBJECTS) $(MAINOBJECTS))&
		@${MAKE} --no-print-directory -C . -f $(GWENDIR)/Projects/$(GWENLIBARCH)/GWEN-Static.make clean
		@${MAKE} --no-print-directory -C . -f $(GWENDIR)/Projects/$(GWENLIBARCH)/Renderer-SDL2.make clean