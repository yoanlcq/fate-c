.PHONY: all clean mrproper re

all:      uvec4.h      uvec3.h      uvec2.h \
	      ivec4.h      ivec3.h      ivec2.h \
           vec4.h       vec3.h       vec2.h \
          dvec4.h      dvec3.h      dvec2.h \
		  wvec4.h      wvec3.h      wvec2.h \
	    ullvec4.h    ullvec3.h    ullvec2.h

clean: 
	rm -f *vec?.h
mrproper: clean all
re: mrproper

PREFIX=FE_MATH_VEXT_
ns=fe_
idir=fate/math/vext

ullvec4.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec4 -Ddim=4 -Dmask=ullvec4 \
		$< > $@
ullvec3.h: genvex.h.m4 ullvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec3 -Ddim=3 -Dvec4=ullvec4 -Dmask=ullvec4 \
		$< > $@
ullvec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec2 -Ddim=2 \
		$< > $@


uvec4.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
uvec3.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec3 -Ddim=3 -Dvec4=uvec4 -Dmask=uvec4 \
		$< > $@
uvec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec2 -Ddim=2 \
		$< > $@


ivec4.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
ivec3.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec3 -Ddim=3 -Dvec4=ivec4 -Dmask=uvec4 \
		$< > $@
ivec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec2 -Ddim=2 \
		$< > $@


vec4.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
vec3.h: genvex.h.m4 uvec4.h vec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec3 -Ddim=3 -Dmask=uvec4 \
		$< > $@
vec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec2 -Ddim=2 \
		$< > $@


dvec4.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec4 -Ddim=4 -Dmask=ullvec4 \
		$< > $@
dvec3.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec3 -Ddim=3 -Dvec4=dvec4 -Dmask=ullvec4 \
		$< > $@
dvec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec2 -Ddim=2 \
		$< > $@


wvec4.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec4 -Ddim=4 -Dmask=wvec4 \
		$< > $@
wvec3.h: genvex.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec3 -Ddim=3 -Dvec4=wvec4 -Dmask=wvec4 \
		$< > $@
wvec2.h: genvex.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec2 -Ddim=2 \
		$< > $@

