u64v4.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=u64v4 -Ddim=4 -Dmask=u64v4 \
		$< > $@
u64v3.h: $(GENVEC_SRC) u64v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=u64v3 -Ddim=3 -Df32v4=u64v4 -Dmask=u64v4 \
		$< > $@
u64v2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint64_t -Dvec=u64v2 -Ddim=2 \
		$< > $@


u32v4.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=u32v4 -Ddim=4 -Dmask=u32v4 \
		$< > $@
u32v3.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=u32v3 -Ddim=3 -Df32v4=u32v4 -Dmask=u32v4 \
		$< > $@
u32v2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=uint32_t -Dvec=u32v2 -Ddim=2 \
		$< > $@


i32v4.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=i32v4 -Ddim=4 -Dmask=u32v4 \
		$< > $@
i32v3.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=i32v3 -Ddim=3 -Df32v4=i32v4 -Dmask=u32v4 \
		$< > $@
i32v2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=int32_t -Dvec=i32v2 -Ddim=2 \
		$< > $@


f32v4.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=f32v4 -Ddim=4 -Dmask=u32v4 \
		$< > $@
f32v3.h: $(GENVEC_SRC) u32v4.h f32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=f32v3 -Ddim=3 -Dmask=u32v4 \
		$< > $@
f32v2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=float -Dvec=f32v2 -Ddim=2 \
		$< > $@


f64v4.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=f64v4 -Ddim=4 -Dmask=u64v4 \
		$< > $@
f64v3.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=f64v3 -Ddim=3 -Df32v4=f64v4 -Dmask=u64v4 \
		$< > $@
f64v2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=double -Dvec=f64v2 -Ddim=2 \
		$< > $@


wv4.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wv4 -Ddim=4 -Dmask=wv4 \
		$< > $@
wv3.h: $(GENVEC_SRC) u32v4.h
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wv3 -Ddim=3 -Df32v4=wv4 -Dmask=wv4 \
		$< > $@
wv2.h: $(GENVEC_SRC)
	m4 --gnu -Dgenerator=$< -Ddir=$(idir) \
		-DPREFIX=$(PREFIX) -Dns=$(ns) \
		-Dtype=fe_space_unit -Dvec=wv2 -Ddim=2 \
		$< > $@

