#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <termios.h>

#define MAX_BUFFER_SIZE   500
#define FTP_SERVER_PORT   21

/* Server response codes */
#define SV_INIT_AUTH        220
#define SV_AUTH_READY       331
#define SV_AUTH_SUCCESS     230
#define SV_PASSIVE_MODE     227
#define SV_TRANSFER_READY   150
#define SV_TRANSFER_DONE    226
#define SV_DISCONNECT       221

/* Parser regular expressions */
#define REGEX_AT             "@"
#define REGEX_SLASH          "/"
#define REGEX_HOST           "%*[^/]//%[^/]"
#define REGEX_HOST_USER      "%*[^/]//%*[^@]@%[^/]"
#define REGEX_RESOURCE       "%*[^/]//%*[^/]/%s"
#define REGEX_USERNAME       "%*[^/]//%[^:/]"
#define REGEX_PASSWORD       "%*[^/]//%*[^:]:%[^@\n$]"
#define REGEX_RESP_CODE      "%d"
#define REGEX_PASSIVE_DATA   "%*[^(](%d,%d,%d,%d,%d,%d)%*[^\n$)]"

/* Default login credentials for anonymous access */
#define ANON_USERNAME        "guest"
#define ANON_PASSWORD        "guest123"

/* Parsed URL information */
struct ConnectionInfo {
    char hostname[MAX_BUFFER_SIZE];
    char filePath[MAX_BUFFER_SIZE];
    char fileName[MAX_BUFFER_SIZE];
    char username[MAX_BUFFER_SIZE];
    char userPassword[MAX_BUFFER_SIZE];
    char serverIP[MAX_BUFFER_SIZE];
};

/* Response parsing states */
typedef enum {
    STATE_START,
    STATE_SINGLE_LINE,
    STATE_MULTI_LINE,
    STATE_END
} ResponseStatus;

/* Function declarations */
int processInput(char *input, struct ConnectionInfo *info);
int initializeConnection(char *serverIP, int serverPort);
int authenticateUser(const int connectionFD, const char *username, const char *password);
int enterPassiveMode(const int connectionFD, char *dataIP, int *dataPort);
int receiveResponse(const int connectionFD, char *responseBuffer);
int requestFile(const int controlFD, char *filePath);
int downloadFile(const int controlFD, const int dataFD, char *fileName);
int terminateConnections(const int controlFD, const int dataFD);
