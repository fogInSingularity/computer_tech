#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(const int argc, const char* argv[]) {
    fprintf(stderr, "Helper entered\n");

    if (argc < 2) {
        fprintf (stderr, "child: too few arguments\n");
        return EXIT_FAILURE;
    }

    int year = strtod(argv[1], NULL);
    fprintf(stderr, "Helper: str: %s, int: %d", argv[1], year);

    if (year <= 0) { return EXIT_FAILURE; }

    int check_year = ((year % 4 == 0) && (year % 100 != 0)) 
                     || (year % 400 == 0);
    if (check_year) { kill(getppid(), SIGUSR1); }
    else            { kill(getppid(), SIGUSR2); }

    return 0;
}
