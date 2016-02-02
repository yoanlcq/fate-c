batch 5 do create foo$(1); 
// Translates to : 
// for(i=0 ; i<5 ; ++i) 
//     create("foo" + i);

batch 4 2 do create foo$(1)$(2); 
// Translates to : 
// for(i=0 ; i<4 ; ++i) 
//     for(j=0 ; j<2 ; ++j) 
//         create("foo" + i + j);

//Copy
foo.x = 42
foo.**.bar.x = foo.x
foo = bar

//Move or rename
foo =< bar
to foo.bar do oldchild =< newchild

//Symlink
foo -> bar

//Hard link
foo => bar

//Unlink. Keyword, for readability.
unlink foo.bar
unlink foo



......

FST code comes in the form of blocks and chains.
A block can contain chains.
A chain consists of several blocks.

Consider this :

as foo.*, bar.* {
    do x=5
    as foobar {
        do y=4
        // The next 'create' chains each do the same thing.
        create exclusive:yes baz, 
               babaz exclusive:no
        create exclusive:yes baz, exclusive:no babaz
        create (
            exclusive:yes baz,
            babaz exclusive:no
        )
        link baz* target:babaz exclusive:yes
        symlink bar* target:baz exclusive:yes
        export bar* baz* (
            format:json
            compress:zip
            storage:persistent
            root:gamepath
            url:"mydir/mydata.json.zip"
        ), babaz* (
            url:"mydir/foo.unknown"
        )
        import foobar (
            url:"mydir/foobar.json.zip"
        )
    }
}
//This will execute the following code paths:
// foreach _ in foo.* 
//     pushd _ -> x=5 -> cd foobar -> y=4 -> create baz babaz -> popd
// foreach _ in bar.* 
//     pushd _ -> x=5 -> cd foobar -> y=4 -> create baz babaz -> popd

// There's no directory for blue collision boxes. Use red instead.
alias hit.* name:_ {
    do _.vel = vel
    do _.vel *= 5
}

Commands :

state
alias
import
export
do
to
as
for
by
