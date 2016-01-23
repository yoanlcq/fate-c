divert(-1)

define(`unavailable_article_en',
`<div class="oops">
<i>
This article is a placeholder. Contributions are welcome !
</i>
</div>')

define(`unavailable_article_fr',
`<div class="oops">
<i>
Cet article n&apos;est pas encore disponible en français.
<a href="index_en.html">Essayez la version anglaise !</a>
</i>
</div>')

define(`unavailable_article',unavailable_article_`'lc)

divert`'dnl
define(`_', `get_prev_')dnl
sinclude(get_prev/`properties.m4')dnl
define(_`title',_`'title_`'lc)dnl
define(`_', `get_')dnl
include(get_path/`properties.m4')dnl
define(_`title',_`'title_`'lc)dnl
define(`_', `get_next_')dnl
sinclude(get_next/`properties.m4')dnl
define(_`title',_`'title_`'lc)dnl
undefine(`_')dnl
include(get_root/`components/html_.m4')dnl
include(get_root/`components/head_.m4')dnl
ifelse(get_prev,,,<link rel="prev" title="get_prev_title()" href="get_prev()/index_`'lc.html"/>)
ifelse(get_next,,,<link rel="next" title="get_next_title()" href="get_next()/index_`'lc.html"/>)
include(get_root/`components/_head.m4')dnl
include(get_root/`components/body_.m4')dnl
