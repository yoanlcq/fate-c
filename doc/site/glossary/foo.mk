
FIRSTLETTERS = $(sort $(shell grep name_fr entries/*.m4 | cut -d \` -f 3-4 | cut -c 1))

define PREP_FILES
$(1)_FILES = $(foreach file,$(wildcard entries/*.m4),$(if $(shell grep name_fr $(file) | cut -d \` -f 3-4 | cut -c 1 | grep $(1)),$(notdir $(basename $(file))) ))
endef

$(foreach i,$(FIRSTLETTERS),$(eval $(call PREP_FILES, $(i))))

# Pour chaque entree
#     Si sa traduction française commence par D
#         Ajouter son nom de base

all:
	m4 -Dget_root=.. -Dlc=fr \
        -Dletters="$(FIRSTLETTERS)" \
		$(foreach i,$(FIRSTLETTERS),-D$(i)_files="$($(i)_FILES)") \
        #index.m4
