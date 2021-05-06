#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include <json-c/json.h>
#define FAIL -1
#define HOST "covid-19-tracking.p.rapidapi.com"
#define PORT 443
SSL *ssl;
int thread_count=0;
int OpenConnection(const char *hostname, int port)
{
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)(host->h_addr);
    if (connect(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}


SSL_CTX *InitCTX(void)
{
    SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();     /* Load cryptos, et.al. */
    SSL_load_error_strings();         /* Bring in and register error messages */
    method = TLSv1_2_client_method(); /* Create new client-method instance */
    ctx = SSL_CTX_new(method);        /* Create new context */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}


void ShowCerts(SSL *ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert != NULL)
    {
        //printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        //printf("Subject: %s\n", line);
        free(line); /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        //printf("Issuer: %s\n", line);
        free(line);      /* free the malloc'ed string */
        X509_free(cert); /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}


void getData(char *country){
    int bytes;
    char line[100000];
    char buf[1024];
    int j=0;
    char str='{';
    int index;
    char *first="GET /v1/";
    char write_buf[2048];

    char *write = " HTTP/1.1\r\n"
    "Host: " HOST "\r\n"
    "Content-Type: application/x-www-form-urlencoded \r\n"
    "x-rapidapi-Key: 93d6478421mshcff35c0e1ff6140p1061cbjsnce6120e66263\r\n"
    "x-rapidapi-Host: covid-19-tracking.p.rapidapi.com\r\n"
    "Connection: close\r\n"
    "\r\n";
    strcat(write_buf,first);
    strcat(write_buf,country);
    strcat(write_buf,write);
    SSL_write(ssl, write_buf, strlen(write_buf)); /* encrypt & send message */

    FILE  *fdJSON;
    char *folder="JSON/";
    char *ext=".json";
    char file_name[15]="";
    strcat(file_name,folder);
    strcat(file_name,country);
    strcat(file_name,ext);
    //Eg.:File_name="JSON/India.json"
    if((fdJSON=fopen(file_name,"w+"))==NULL){
        printf("Error in file\n");
    }
    for (;;)
    {
            //  Get chunks of the response 1023 at the time.
            
                bytes = SSL_read(ssl, buf, sizeof(buf)); /* get reply & decrypt */
           
            //  If no more data, then exit the loop
        if (bytes <= 0)
        {
            break;
        }
        buf[bytes] = 0;

        for(int i=0;i<strlen(buf);i++){
            if(str==buf[i]){
                index=i;
                break;
            }
        }

        for(int i=index;i<strlen(buf);i++){
            line[j]=buf[i];
            j++;
        }

    }


    fputs(line,fdJSON);
    fclose(fdJSON);
}
int main(int count, char *strings[])
{
    SSL_CTX *ctx;
    int server;    
    
    char acClientRequest[1024] = {0};
    if (count != 2)
    {
        printf("usage: %s <country name>\n", strings[0]);
        exit(0);
    }
    SSL_library_init();
    ctx = InitCTX();
    server = OpenConnection(HOST, PORT);
    ssl = SSL_new(ctx);           /* create new SSL connection state */
    SSL_set_fd(ssl, server);      /* attach the socket descriptor */
    if (SSL_connect(ssl) == FAIL) /* perform the connection */
    ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl);
        getData(strings[1]);/*Send the name of the country to fetch data*/
    }

    SSL_free(ssl); /* release connection state */
    close(server);     /* close socket */
    SSL_CTX_free(ctx); /* release context */

    exit(0) ;
}