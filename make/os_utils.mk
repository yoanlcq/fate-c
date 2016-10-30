ifeq ($(host_os),windows)
mkdir_p = if not exist $(subst /,\\,$(1)) ( mkdir $(subst /,\\,$(1)) )
cp_r = xcopy /s /e $(subst /,\\,$(1)) $(subst /,\\,$(2))
copyfiles = copy $(subst /,\\,$(1)) $(subst /,\\,$(2))
movefiles = move $(subst /,\\,$(1)) $(subst /,\\,$(2))
ln_sf = mklink $(subst /,\\,$(2)) $(subst /,\\,$(1))
rmdir_rf = if exist $(subst /,\\,$(1)) ( rmdir /q /s $(subst /,\\,$(1)) )
echo = echo $(1)
cat = type $(1)
pathsep=\\
dot_exe=.exe
dll_prefix=
dot_dll=.dll
else
mkdir_p = mkdir -p $(1)
cp_r = cp -pR $(1) $(2)
copyfiles = cp $(1) $(2)
movefiles = mv $(1) $(2)
ln_sf = ln -sf $(1) $(2)
rmdir_rf = rm -rf $(1)
echo = echo $(1)
cat = cat $(1)
pathsep=/
dot_exe=
dll_prefix=lib
dot_dll=.so
endif
readfile = $(shell $(call cat, $(1)))
