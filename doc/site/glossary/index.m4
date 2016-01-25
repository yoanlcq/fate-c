include(get_root/`m4_utils/foreach2.m4')dnl

### components/glossary_.m4 :

# Typical interpretation command :
# m4 -Dget_root=.. -Dlc=fr \
#     -Dletters="(D,E,R)" \
#     -DD_files="(DirectFieldAccess)" \
#     -DE_files="(Entity)" \
#     -DR_files="(PathResolution)" \
#     index.m4
#
# There's no mistake in the above command. 
# In French, PathResolution is "Résolution de chemins".
#

foreach(`letter', letters, `
    define(`files', letter`'_files)
    foreach(`file', files, `
        define(`_',`glossary_'file`_')
        include(`entries/'file`.m4')
        define(_`name',_`name_'lc)
        define(_`hint',_`hint_'lc)
        define(_`desc',_`desc_'lc)
    ')
    undefine(`files')
')
undefine(`_')

dnl We put dnls everywhere because the result must fit in a Javascript string.
dnl There's no indentation either, for the same reasons.
dnl I do acknowledge it looks like a huge mess without it.
dnl Dear maintainer, please stay determined.
define(`glossary_make_elem',
`<div class="glossary_entry">dnl
<h1 class="glossary_name">glossary_$1_name</h1>dnl
<p class="glossary_hint">glossary_$1_hint</p>dnl
<div class="glossary_desc">glossary_$1_desc</div>dnl
ifelse(glossary_$1_related,`()',,glossary_$1_related,`',,`dnl
<i>dnl
ifelse(lc,en,`Related',lc,fr,`Voir aussi') : dnl
define(`m_comma',`')dnl
changecom()dnl
foreach(`lnk', glossary_$1_related, `dnl
define(`mm_name',glossary_`'lnk`'_name)dnl
m_comma <a href="/glossary/index_`'lc.html#lnk">mm_name</a>dnl
undefine(`mm_name')dnl
define(`m_comma',`,')dnl
')dnl
')dnl
changecom(`#')dnl
undefine(`m_comma')dnl
`</i>'dnl
</div>')



### index.m4 :

changecom()dnl Disable comments.

foreach(`letter', letters, `dnl
ifelse(parity,even,`define(`parity',`odd')',`define(`parity',`even')')dnl
    <a href="#letter" class="parity">letter</a>
')
undefine(`parity')dnl

foreach(`letter', letters, `
    <h1 id="letter">letter</h1>
    <ul>
    define(`files', letter`'_files)dnl
    foreach(`file', files, `dnl
        define(`name',glossary_`'file`'_name)dnl
<li><a href="#file">name</a></li>dnl
        undefine(`name')dnl
    ')dnl
    undefine(`files')
    </ul>
')
changecom(`#')dnl

foreach(`letter', letters, `
    define(`files', letter`'_files)
    foreach(`file', files, `
        <div id="file">glossary_make_elem(file)</div>
    ')
    undefine(`files')
')


#### entries_lc.js.m4

var glossary_entries = [
foreach(`letter', letters, `dnl
define(`files', letter`'_files)dnl
foreach(`file', files, `dnl
define(`m_name',glossary_`'file`'_name)dnl
    {
        name:"m_name",
        elem:"glossary_make_elem(file)"
    },
undefine(`m_name')dnl
')dnl
undefine(`files')dnl
')dnl
    { 
        name:"", 
        elem:""
    }
];
