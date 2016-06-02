void remove_C_comments_and_preprocessor_directives(char *src) {
    for( ; *src ; ++src) {
        switch(*src) {
        case '\'': 
            if(src[1]=='\\' && src[2]=='\'')
                src += 3;
            else do ++src; while(*src!='\'');
            break;
        case '"':
            do ++src; while(!(*src=='"' && src[-1]!='\\'));
            break;
        case '/':
            if(src[1]=='/') {
                src[0] = src[1] = ' ';
                ++src;
                for(++src ; *src!='\n' ; ++src)
                    *src = ' ';
            } else if(src[1]=='*') {
                src[0] = src[1] = ' ';
                ++src;
                for(++src ; !(*src=='*' && src[1]=='/') ; ++src)
                    *src = ' ';
                src[0] = src[1] = ' ';
                ++src;
            }
            break;
        case '#':
            *src = ' ';
            for(++src ; *src!='\n' ; ++src) {
                if(*src=='\\' && src[1]=='\n') {
                    src[0] = src[1] = ' ';
                    ++src;
                } else *src = ' ';
            }
            break;
        }
    }
}

