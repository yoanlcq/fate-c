.PHONY: all clean mrproper re

all: vext_files \
	      uvec4.h      uvec3.h      uvec2.h \
	      ivec4.h      ivec3.h      ivec2.h \
           vec4.h       vec3.h       vec2.h \
          dvec4.h      dvec3.h      dvec2.h \
	  spacevec4.h  spacevec3.h  spacevec2.h

clean: 
	rm -f *vec?.h
mrproper: clean all
re: mrproper



vext_files:
	$(MAKE) $(MAKEFLAGS) -C vext $(MAKECMDGOALS)

uvec4.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=uint32_t -Dvec=uvec4 -Ddim=4 \
		$< > $@
uvec3.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=uint32_t -Dvec=uvec3 -Ddim=3 -Dvec4=uvec4 \
		$< > $@
uvec2.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=uint32_t -Dvec=uvec2 -Ddim=2 \
		$< > $@


ivec4.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=int32_t -Dvec=ivec4 -Ddim=4 \
		$< > $@
ivec3.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=int32_t -Dvec=ivec3 -Ddim=3 -Dvec4=ivec4 \
		$< > $@
ivec2.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=int32_t -Dvec=ivec2 -Ddim=2 \
		$< > $@


vec4.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=float -Dvec=vec4 -Ddim=4 \
		$< > $@
vec3.h: genvec.h.m4 uvec4.h vec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=float -Dvec=vec3 -Ddim=3 \
		$< > $@
vec2.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=float -Dvec=vec2 -Ddim=2 \
		$< > $@


dvec4.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=double -Dvec=dvec4 -Ddim=4 \
		$< > $@
dvec3.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=double -Dvec=dvec3 -Ddim=3 -Dvec4=dvec4 \
		$< > $@
dvec2.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=double -Dvec=dvec2 -Ddim=2 \
		$< > $@


spacevec4.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=fate_space_unit -Dvec=spacevec4 -Ddim=4 \
		$< > $@
spacevec3.h: genvec.h.m4 uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=fate_space_unit -Dvec=spacevec3 -Ddim=3 -Dvec4=spacevec4 \
		$< > $@
spacevec2.h: genvec.h.m4
	m4 --gnu -Dgenerator=$< -Ddir=fate/math \
		-Dprefix=FATE_MATH_ -Dsuffix="" \
		-Dtype=fate_space_unit -Dvec=spacevec2 -Ddim=2 \
		$< > $@

