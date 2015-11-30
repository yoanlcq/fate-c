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

BUILDDIRS = $(BUILDDIR) \
		    $(BUILDDIR)$(PATHSEP)fate \
            $(BUILDDIR)$(PATHSEP)fate$(PATHSEP)gl \
            $(BUILDDIR)$(PATHSEP)cube \


dirs: $(BINDIR) \
	  $(BUILDDIRS) \
	  $(DATADIR) \
	  $(DATADIR)$(PATHSEP)OpenGL

MKDIR = mkdir
ifneq ($(OS),windows)
MKDIR += -p
CLEANCMD = rm -rf $(BUILDDIR)/*
else
CLEANCMD = rmdir /q /s $(BUILDDIR)\*
endif

clean:
	$(CLEANCMD)
mrproper : clean all
re : mrproper



$(BINDIR):
	$(MKDIR) $@
$(BUILDDIR):
	$(MKDIR) $@
$(BUILDDIR)$(PATHSEP)fate:
	$(MKDIR) $@
$(BUILDDIR)$(PATHSEP)fate$(PATHSEP)gl:
	$(MKDIR) $@
$(BUILDDIR)$(PATHSEP)cube:
	$(MKDIR) $@
$(DATADIR):
	$(MKDIR) $@
$(DATADIR)$(PATHSEP)OpenGL:
	$(MKDIR) $@

