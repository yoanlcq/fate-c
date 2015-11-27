ifeq ($(OS),windows)
BUILDDIR = build\$(OS)$(ARCH)
else
BUILDDIR = build/$(OS)$(ARCH)
endif
BINDIR = bin/$(OS)$(ARCH)
DATADIR = data

ifneq ($(OS),windows)
MKDIR = mkdir -p
CLEANCMD = rm -f $(BUILDDIR)/*
else
MKDIR = mkdir
CLEANCMD = del /f /q $(BUILDDIR)\*
endif

dirs: $(BINDIR) $(BUILDDIR) $(DATADIR) $(DATADIR)/OpenGL

$(BINDIR):
	$(MKDIR) $(BINDIR)
$(BUILDDIR):
	$(MKDIR) $(BUILDDIR)
$(DATADIR):
	$(MKDIR) $(DATADIR)
$(DATADIR)/OpenGL:
	$(MKDIR) $(DATADIR)/OpenGL

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper
