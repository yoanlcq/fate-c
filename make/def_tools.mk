triplet:=$(os)$(arch)_$(cc_id)
cc:=$(triplet)_cc
as:=$(triplet)_as
fe_build_dir:=$(fate)/build/$(triplet)/$(build)/
gm_build_dir:=$(dirs_obj)/$(triplet)/$(build)/