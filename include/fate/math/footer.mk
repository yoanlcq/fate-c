ullvec4.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec4 -Ddim=4 -Dmask=ullvec4 \
		$< > $@
ullvec3.h: $(GENVEC_SRC) ullvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec3 -Ddim=3 -Dvec4=ullvec4 -Dmask=ullvec4 \
		$< > $@
ullvec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=ullvec2 -Ddim=2 \
		$< > $@


uvec4.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
uvec3.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec3 -Ddim=3 -Dvec4=uvec4 -Dmask=uvec4 \
		$< > $@
uvec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=uvec2 -Ddim=2 \
		$< > $@


ivec4.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
ivec3.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec3 -Ddim=3 -Dvec4=ivec4 -Dmask=uvec4 \
		$< > $@
ivec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=ivec2 -Ddim=2 \
		$< > $@


vec4.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec4 -Ddim=4 -Dmask=uvec4 \
		$< > $@
vec3.h: $(GENVEC_SRC) uvec4.h vec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec3 -Ddim=3 -Dmask=uvec4 \
		$< > $@
vec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=vec2 -Ddim=2 \
		$< > $@


dvec4.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec4 -Ddim=4 -Dmask=ullvec4 \
		$< > $@
dvec3.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec3 -Ddim=3 -Dvec4=dvec4 -Dmask=ullvec4 \
		$< > $@
dvec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=dvec2 -Ddim=2 \
		$< > $@


wvec4.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec4 -Ddim=4 -Dmask=wvec4 \
		$< > $@
wvec3.h: $(GENVEC_SRC) uvec4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec3 -Ddim=3 -Dvec4=wvec4 -Dmask=wvec4 \
		$< > $@
wvec2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wvec2 -Ddim=2 \
		$< > $@

