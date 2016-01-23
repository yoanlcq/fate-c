batch 5 do create foo$(1); 
// Translates to : 
// for(i=0 ; i<5 ; ++i) 
//     create("foo" + i);

batch 4 2 do create foo$(1)$(2); 
// Translates to : 
// for(i=0 ; i<4 ; ++i) 
//     for(j=0 ; j<2 ; ++j) 
//         create("foo" + i + j);
