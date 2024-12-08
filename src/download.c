#include "download.h"

int processInput(char *input, struct ConnectionInfo *info) {
    regex_t regex;
    regcomp(&regex, REGEX_SLASH, 0);
    if (regexec(&regex, input, 0, NULL, 0)) return -1;

    regcomp(&regex, REGEX_AT, 0);
    if (regexec(&regex, input, 0, NULL, 0) != 0) { 
        sscanf(input, REGEX_HOST, info->hostname);
        strcpy(info->username, ANON_USERNAME);
        strcpy(info->userPassword, ANON_PASSWORD);
    } else {
        sscanf(input, REGEX_HOST_USER, info->hostname);
        sscanf(input, REGEX_USERNAME, info->username);
        sscanf(input, REGEX_PASSWORD, info->userPassword);
    }

    sscanf(input, REGEX_RESOURCE, info->filePath);
    strcpy(info->fileName, strrchr(input, '/') + 1);

    struct hostent *hostEntity;
    if (strlen(info->hostname) == 0) return -1;
    if ((hostEntity = gethostbyname(info->hostname)) == NULL) {
        printf("Invalid hostname '%s'\n", info->hostname);
        exit(-1);
    }
    strcpy(info->serverIP, inet_ntoa(*((struct in_addr *) hostEntity->h_addr)));

    return !(strlen(info->hostname) && strlen(info->username) && 
           strlen(info->userPassword) && strlen(info->filePath) && strlen(info->fileName));
}

int initializeConnection(char *serverIP, int serverPort) {
    int connectionFD;
    struct sockaddr_in serverAddress;

    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverIP);  
    serverAddress.sin_port = htons(serverPort); 
    
    if ((connectionFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating socket");
        exit(-1);
    }
    if (connect(connectionFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error connecting to server");
        exit(-1);
    }
    return connectionFD;
}

int authenticateUser(const int connectionFD, const char *username, const char *password) {
    char userCommand[6 + strlen(username)];
    sprintf(userCommand, "user %s\n", username);

    char passCommand[6 + strlen(password)];
    sprintf(passCommand, "pass %s\n", password);

    char response[MAX_BUFFER_SIZE];
    write(connectionFD, userCommand, strlen(userCommand));
    if (receiveResponse(connectionFD, response) != SV_AUTH_READY) {
        printf("Unknown user '%s'. Abort.\n", username);
        exit(-1);
    }

    write(connectionFD, passCommand, strlen(passCommand));
    return receiveResponse(connectionFD, response);
}

int enterPassiveMode(const int connectionFD, char *dataIP, int *dataPort) {
    char response[MAX_BUFFER_SIZE];
    int ip1, ip2, ip3, ip4, port1, port2;

    write(connectionFD, "pasv\n", 5);
    if (receiveResponse(connectionFD, response) != SV_PASSIVE_MODE) return -1;

    sscanf(response, REGEX_PASSIVE_DATA, &ip1, &ip2, &ip3, &ip4, &port1, &port2);
    *dataPort = port1 * 256 + port2;
    sprintf(dataIP, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

    return SV_PASSIVE_MODE;
}

int receiveResponse(const int connectionFD, char *responseBuffer) {
    char byte;
    int index = 0, responseCode;
    ResponseStatus state = STATE_START;
    memset(responseBuffer, 0, MAX_BUFFER_SIZE);

    while (state != STATE_END) {
        read(connectionFD, &byte, 1);
        switch (state) {
            case STATE_START:
                if (byte == ' ') state = STATE_SINGLE_LINE;
                else if (byte == '-') state = STATE_MULTI_LINE;
                else if (byte == '\n') state = STATE_END;
                else responseBuffer[index++] = byte;
                break;
            case STATE_SINGLE_LINE:
                if (byte == '\n') state = STATE_END;
                else responseBuffer[index++] = byte;
                break;
            case STATE_MULTI_LINE:
                if (byte == '\n') {
                    memset(responseBuffer, 0, MAX_BUFFER_SIZE);
                    state = STATE_START;
                    index = 0;
                } else responseBuffer[index++] = byte;
                break;
            case STATE_END:
                break;
        }
    }

    sscanf(responseBuffer, REGEX_RESP_CODE, &responseCode);
    return responseCode;
}

int requestFile(const int controlFD, char *filePath) {
    char command[6 + strlen(filePath)];
    sprintf(command, "retr %s\n", filePath);

    char response[MAX_BUFFER_SIZE];
    write(controlFD, command, sizeof(command));
    return receiveResponse(controlFD, response);
}

int downloadFile(const int controlFD, const int dataFD, char *fileName) {
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        printf("Error opening file '%s'\n", fileName);
        exit(-1);
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;
    do {
        bytesRead = read(dataFD, buffer, MAX_BUFFER_SIZE);
        if (fwrite(buffer, bytesRead, 1, file) < 0) return -1;
    } while (bytesRead);

    fclose(file);

    char response[MAX_BUFFER_SIZE];
    return receiveResponse(controlFD, response);
}

int terminateConnections(const int controlFD, const int dataFD) {
    char response[MAX_BUFFER_SIZE];
    write(controlFD, "quit\n", 5);
    if (receiveResponse(controlFD, response) != SV_DISCONNECT) return -1;
    return close(controlFD) || close(dataFD);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    struct ConnectionInfo info;
    memset(&info, 0, sizeof(info));
    if (processInput(argv[1], &info) != 0) {
        printf("Parse error. Usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(-1);
    }

    printf("Host: %s\nResource: %s\nFile: %s\nUser: %s\nPassword: %s\nIP Address: %s\n", info.hostname, info.filePath, info.fileName, info.username, info.userPassword, info.serverIP);

    int controlFD = initializeConnection(info.serverIP, FTP_SERVER_PORT);
    char responseBuffer[MAX_BUFFER_SIZE];
    if (controlFD < 0 || receiveResponse(controlFD, responseBuffer) != SV_INIT_AUTH) {
        printf("Unable to establish a connection to '%s' on port %d\n", info.serverIP, FTP_SERVER_PORT);
        exit(-1);
    }

    if (authenticateUser(controlFD, info.username, info.userPassword) != SV_AUTH_SUCCESS) {
        printf("Authentication unsuccessful using username '%s' and password '%s'.\n", info.username, info.userPassword);
        exit(-1);
    }

    int dataPort;
    char dataIP[MAX_BUFFER_SIZE];
    if (enterPassiveMode(controlFD, dataIP, &dataPort) != SV_PASSIVE_MODE) {
        printf("Passive mode initialization failed.\n");
        exit(-1);
    }

    int dataFD = initializeConnection(dataIP, dataPort);
    if (dataFD < 0) {
        printf("Unable to connect to '%s' on port %d.\n", dataIP, dataPort);
        exit(-1);
    }

    if (requestFile(controlFD, info.filePath) != SV_TRANSFER_READY) {
        printf("Resource '%s' could not be found on '%s:%d'.\n", info.filePath, dataIP, dataPort);
        exit(-1);
    }

    if (downloadFile(controlFD, dataFD, info.fileName) != SV_TRANSFER_DONE) {
        printf("Failed to retrieve '%s' from '%s:%d'.\n", info.fileName, dataIP, dataPort);
        exit(-1);
    }

    if (terminateConnections(controlFD, dataFD) != 0) {
        printf("Error closing connections.\n");
        exit(-1);
    }

    printf("Download successful.\n");
    return 0;
}
