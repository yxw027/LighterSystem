#include <stdio.h>
#include<unistd.h>

int main()
{
    char *args[] = {"./OpenHellGate" , NULL };
    
    execvp(args[0],args);

    return 0;
}