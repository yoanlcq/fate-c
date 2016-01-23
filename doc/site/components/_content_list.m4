end
undefine(`path',`root',`_',`pushd',`popd',`end')

include(`m4_utils/foreach2.m4')
include(`m4_utils/forloop3.m4')
define(`FILES',)dnl
define(`GOALS',)dnl
forloop(`i', `1', eval(cnt-1), `dnl
    foreach(`lc', `(en, fr)', `
define(`m_prev',prev`_'i)dnl
define(`m_next',next`_'i)dnl
define(`m_root',root`_'i)dnl
define(`m_path',path`_'i)dnl
define(`FILES',FILES m_path/index_`'lc.html)dnl
define(`GOALS',GOALS 
m_path/index_`'lc.html: ifelse(m_prev,,,m_prev/properties.m4) \
                      m_path/properties.m4 \
                      ifelse(m_next,,,m_next/properties.m4) \
                      m_path/index.m4 \
                      m_path/content_`'lc.html
	m4 --include=m_path -Dlc=lc \
        -Dget_prev=ifelse(m_prev,,"",m_prev) \
        -Dget_next=ifelse(m_next,,"",m_next) \
        -Dget_path=m_path -Dget_root=m_root \
        m_path/index.m4 > m_path/index_`'lc.html
)
')
')
undefine(`m_prev', `m_next', `m_root', `m_path')dnl

divert`'dnl

# This Makefile was generated automagically via M4.
# See 'content_list.m4'.

.PHONY: all clean re mrproper

all: FILES()

GOALS()

clean:
	rm -f FILES()

re: clean all

mrproper: re
undefine(`FILES',`GOALS')dnl
