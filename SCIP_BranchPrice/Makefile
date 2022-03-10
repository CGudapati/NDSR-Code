SCIPDIR         =       /Library/scip7.0.1/scip


#-----------------------------------------------------------------------------
# include default project Makefile from SCIP (need to do this twice, once to
# find the correct binary, then, after getting the correct flags from the
# binary (which is necessary since the ZIMPL flags differ from the default
# if compiled with the SCIP Optsuite instead of SCIP), we need to set the
# compile flags, e.g., for the ZIMPL library, which is again done in make.project
#-----------------------------------------------------------------------------
include $(SCIPDIR)/make/make.project
SCIPVERSION			:=$(shell $(SCIPDIR)/bin/scip.$(BASE).$(LPS).$(TPI)$(EXEEXTENSION) -v | sed -e 's/$$/@/')
override ARCH			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ARCH=\([^@]*\).*/\1/')
override EXPRINT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* EXPRINT=\([^@]*\).*/\1/')
override GAMS			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* GAMS=\([^@]*\).*/\1/')
override GMP			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* GMP=\([^@]*\).*/\1/')
override SYM			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* SYM=\([^@]*\).*/\1/')
override IPOPT			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* IPOPT=\([^@]*\).*/\1/')
override IPOPTOPT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* IPOPTOPT=\([^@]*\).*/\1/')
override LPSCHECK		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* LPSCHECK=\([^@]*\).*/\1/')
override LPSOPT 		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* LPSOPT=\([^@]*\).*/\1/')
override NOBLKBUFMEM		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBLKBUFMEM=\([^@]*\).*/\1/')
override NOBLKMEM		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBLKMEM=\([^@]*\).*/\1/')
override NOBUFMEM		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* NOBUFMEM=\([^@]*\).*/\1/')
override PARASCIP		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* PARASCIP=\([^@]*\).*/\1/')
override READLINE		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* READLINE=\([^@]*\).*/\1/')
override SANITIZE		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* SANITIZE=\([^@]*\).*/\1/')
override ZIMPL			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZIMPL=\([^@]*\).*/\1/')
override ZIMPLOPT		:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZIMPLOPT=\([^@]*\).*/\1/')
override ZLIB			:=$(shell echo "$(SCIPVERSION)" | sed -e 's/.* ZLIB=\([^@]*\).*/\1/')
include $(SCIPDIR)/make/make.project

#-----------------------------------------------------------------------------
# Main Program
#-----------------------------------------------------------------------------

MAINNAME	=	NDSR_only_colgen_MM_pricer_cutoff_w_presol
MAINOBJ		=	NDSR_ColGen_main.o NDSR_Instance.o pricer_NDSR.o Timer.o   RCSPP_graph.o
MAINSRC		=	$(addprefix $(SRCDIR)/,$(MAINOBJ:.o=.cpp))
MAIN		=	$(MAINNAME).$(BASE).$(LPS)$(EXEEXTENSION)
MAINFILE	=	$(BINDIR)/$(MAIN)
MAINSHORTLINK	=	$(BINDIR)/$(MAINNAME)
MAINOBJFILES	=	$(addprefix $(OBJDIR)/,$(MAINOBJ))
INCLUDE  := -I$(SRCDIR)/

.PHONY: all
all:            $(SCIPDIR) $(MAINFILE) $(MAINSHORTLINK)

.PHONY: scip
scip:
		@$(MAKE) -C $(SCIPDIR) libs $^

$(MAINSHORTLINK):	$(MAINFILE)
		@rm -f $@
		cd $(dir $@) && ln -s $(notdir $(MAINFILE)) $(notdir $@)

$(OBJDIR):
		@-mkdir -p $(OBJDIR)

$(BINDIR):
		@-mkdir -p $(BINDIR)



.PHONY: run
run:
	./$(BINDIR)/$(MAINNAME)  ${ARGS}

.PHONY: test
test:
	./$(BINDIR)/$(MAINNAME)  data/RBWQN30A120C90W2MM_S1943_net.txt
	./$(BINDIR)/$(MAINNAME)  data/RBWQN50A150C150W2MM_S8958_net.txt			


.PHONY: etest
etest:
	./$(BINDIR)/$(MAINNAME)  data/RBWQN30A120C90W2MM_S6703_net.txt
	./$(BINDIR)/$(MAINNAME)  data/RBWQN40A160C120W2MM_S2415_net.txt			

.PHONY: stest
stest:
	./$(BINDIR)/$(MAINNAME)  data/RBWQN30A120C90W2MM_S5138_net.txt

.PHONY: 1287
1287:
	./$(BINDIR)/$(MAINNAME)  data/RBWQN50A200C100W2MM_S1287_net.txt



$(MAINFILE):	$(BINDIR) $(OBJDIR) $(SCIPLIBFILE) $(LPILIBFILE) $(NLPILIBFILE) $(MAINOBJFILES)
		@echo "-> linking $@"
		$(LINKCXX) $(MAINOBJFILES) $(INCLUDE) $(LINKCXXSCIPALL) $(LDFLAGS)  $(LINKCXX_o)$@


$(OBJDIR)/NDSR_ColGen_main.o: $(SRCDIR)/NDSR_ColGen_main.cpp $(SRCDIR)/NDSR_Instance.hpp $(SRCDIR)/NDSR_Reader.h $(SRCDIR)/NDSR_ColGen_Solver.h  $(SRCDIR)/helper.h   $(SRCDIR)/Graph_Reader.h $(SRCDIR)/get_RCSPP_for_all_commodities.h
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(INCLUDE) $(BINOFLAGS) $(CXXFLAGS)  -c $< $(CXX_o)$@

$(OBJDIR)/NDSR_Instance.o: $(SRCDIR)/NDSR_Instance.cpp	$(SRCDIR)/NDSR_Instance.hpp $(SRCDIR)/helper.h
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(INCLUDE) $(BINOFLAGS) $(CXXFLAGS)  -c  $< $(CXX_o)$@


$(OBJDIR)/Timer.o:	$(SRCDIR)/Timer.cpp $(SRCDIR)/Timer.hpp
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(INCLUDE) $(BINOFLAGS) $(CXXFLAGS)  -c  $< $(CXX_o)$@

$(OBJDIR)/pricer_NDSR.o: $(SRCDIR)/pricer_NDSR.cpp	$(SRCDIR)/pricer_NDSR.hpp $(SRCDIR)/NDSR_Instance.hpp $(SRCDIR)/helper.h $(SRCDIR)/get_RCSPP_for_all_commodities.h 
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(INCLUDE) $(BINOFLAGS) $(CXXFLAGS)  -c  $< $(CXX_o)$@

$(OBJDIR)/RCSPP_graph.o: $(SRCDIR)/RCSPP_graph.cpp	$(SRCDIR)/RCSPP_graph.hpp 
		@echo "-> compiling $@"
		$(CXX) $(FLAGS) $(OFLAGS) $(INCLUDE) $(BINOFLAGS) $(CXXFLAGS)  -c  $< $(CXX_o)$@
