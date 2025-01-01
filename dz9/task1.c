#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/* Полюбоваться на созданный сокет man 2 socket man 2 bind*/
int main (const int argc, const char* argv[argc + 1]) {
	if (argc < 2) {
		fprintf(stderr, "Too few arguments\n");

		return EXIT_FAILURE;
	}

    // open socket
	int sock = socket(/* unix domain */ AF_UNIX, SOCK_STREAM, /* default protocol*/ 0);
	if (sock == -1) {
		perror("socket error:");

        return EXIT_FAILURE;
	}

    struct sockaddr_un saddr;
	saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, argv[1]);
	/* Присвоить массив в структуре нельзя, но можно скопировать содержимое.*/ 
    // saddr.sun_path = *argv[1];
    
    // assign name to socket
	if (bind(sock, (struct sockaddr*)&saddr, SUN_LEN(&saddr)) == -1) {
	    perror("bind error:");

        return EXIT_FAILURE;
	}

	fprintf(stderr, "Press <Enter> to continue...");
	fgetc(stdin);

	int cl = close(sock); // deallocates socket
    if (cl < 0) {
        perror("cant close socket:");
    }

	int un = unlink(argv[1]);
    if (un < 0) {
        perror("cant unlink socket name:");
    }

	return 0;
}
