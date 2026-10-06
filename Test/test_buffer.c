#include "App/buffer.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>


Buffer buf;

char str[30];

int main()
{   
    app_buffer_init(&buf,32);
    
    app_buffer_write(&buf,"hello,world!",12);

    assert(buf.len == 12);
    assert(buf.start == 0);

    app_buffer_read(&buf,str,7);

    assert(buf.len == 5);
    assert(buf.start == 7);
    
    app_buffer_write(&buf,"EAT cake!",9);

    assert(buf.len == 14);
    assert(buf.start == 7);

    app_buffer_read(&buf,str,14);

    assert(buf.len == 0);
    assert(buf.start == 21);

    printf("str: %s\n",str);
    assert(strncmp(str,"orld!EAT cake!",14)==0);
    return 0;
}
