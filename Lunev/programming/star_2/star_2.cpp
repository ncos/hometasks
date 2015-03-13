
#include "receiver.cpp"
#include "sender.cpp"


int main (int argc, char** argv)
{
    receiverpid = getpid();
    senderpid = fork();

    if (argc != 2)
    {
        printf("Usage: star_2 path_to_file\n");
        exit (EXIT_FAILURE);
    }

    if (senderpid == -1)
    {
        perror ("fork");
        exit (EXIT_FAILURE);
    }


	if (senderpid == 0) sender(argv[1]);
    else receiver();

	wait(NULL);


    return 0;
}

