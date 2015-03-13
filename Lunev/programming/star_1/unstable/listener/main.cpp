
#include "listener.h"


int main(int argc, char *argv[])
{
    Listener listener;

    listener.set_connection();
    listener.begin_transmission();

    return 0;
}

