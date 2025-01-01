#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUF_LEN		4096
#define HTTP_PORT	80

void DumpHostent(struct hostent* hent);

/* Получить и распечатать какую либо страничку из интернета man 3 gethostbyname */
int main (const int argc, const char* argv[argc + 1]) {
    fprintf(stderr, "my pid: %d\n", getpid());
	if (argc < 2) {
		fprintf (stderr, "Too few arguments\n");

		return EXIT_FAILURE;
	}

	char* buf = (char*)malloc(BUF_LEN);
	if (buf == NULL) {
		fprintf(stderr, "malloc() error\n");

        return EXIT_FAILURE;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0); // open socket
	if (sock == -1) {
		perror("socket() error");

		return EXIT_FAILURE;
	}

    // int bd = bind(sock, ); // FIXME

    // sethostent(true);
    struct hostent* host = gethostent(); // FIXME
    // struct hostent* host = gethostbyname("google.com");
	if (host == NULL) 
	{
        herror("gethostent error:");

		return EXIT_FAILURE;
	}

    DumpHostent(host);    

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
	    .sin_addr   = *(struct in_addr*)host->h_addr_list[0],
	    .sin_port   = HTTP_PORT,
    };

    // struct sockaddr_in addr = {
    //     .sin_family = AF_INET,
    //     .sin_port   = htons(8080),
	   //  .sin_addr.s_addr = inet_addr("127.0.0.1"),
    // };

	if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("connect() error");
        close(sock);

        // fputs(strerror(ECONNREFUSED), stderr);

        return EXIT_FAILURE;
	}
	
	strcpy(buf, "GET /");
    strcat(buf, " HTTP/1.1\nHost: ");
    strcat(buf, argv[1]);
    strcat(buf, "\n\n");
//strcat(buf, "\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:50.0) Gecko/20100101 Firefox/50.0");
//strcat(buf, "\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
//strcat(buf, "\nAccept-Language: en-US,en;q=0.5");
//strcat(buf, "\nAccept-Encoding: gzip, deflate");
//strcat(buf, "\nConnection: keep-alive\n\n");
	
	write(sock, buf, strlen (buf));
	
    int count = 0;
	while ((count = read(sock, buf, BUF_LEN)) > 0) {
        fwrite(buf, count, sizeof(char), stdout);
    }
		// write(fileno(stdout), buf, count);

	close(sock);
	free(buf);
	return 0;
}

void DumpHostent(struct hostent* hent) {
    assert(hent != NULL);

    fprintf(stderr, "host name: %s\n", hent->h_name);
    for (int i = 0; hent->h_aliases[i] != NULL; i++) {
        fprintf(stderr, "host alias: %s\n", hent->h_aliases[i]);
    }

    switch(hent->h_addrtype) {
        case AF_INET:
            fprintf(stderr, "host addrtype: AF_INET\n");
            break;
        case AF_INET6:
            fprintf(stderr, "host addrtype: AF_INET6\n");
            break;
        default:
            fprintf(stderr, "host addrtype doenst present\n");
            break;
    }
}
