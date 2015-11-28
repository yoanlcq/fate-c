# Using PATHSEP is only required in this file's scope because it is here that
# 'mkdir', for instance, is called, and that on Windows, the items in the
# given path must be separated by '\' instead of '/'.
# However, any utility that is not part of Window's shell doesn't 
# mind the difference.
# Truly, we do this here just to make 'del' and 'mkdir' happy.

ifeq ($(OS),windows)
PATHSEP = \\
else
PATHSEP = /
endif
BUILDDIR = build$(PATHSEP)$(OS)$(ARCH)
BINDIR = bin$(PATHSEP)$(OS)$(ARCH)
DATADIR = data

MKDIR = mkdir
ifneq ($(OS),windows)
MKDIR += -p
CLEANCMD = rm -f $(BUILDDIR)/*
else
CLEANCMD = del /f /q $(BUILDDIR)\*
endif

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper


dirs: $(BINDIR) $(BUILDDIR) $(DATADIR) $(DATADIR)$(PATHSEP)OpenGL

$(BINDIR):
	$(MKDIR) $@
$(BUILDDIR):
	$(MKDIR) $@
$(DATADIR):
	$(MKDIR) $@
$(DATADIR)$(PATHSEP)OpenGL:
	$(MKDIR) $@

