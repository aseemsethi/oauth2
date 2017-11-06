#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oauth2.h"

char* oauth2RequestAuthCode(oauth2Struct *os, char* authServer, char* scope, char* state) {

}

oauth2ConfigureRedirectUrl(oauth2Struct *os, char* url) {
	int len;
	len = strlen(url) + 1;
	os->redirectUrl = malloc(len * sizeof(char));
	strcpy(os->redirectUrl, url);

}

oauth2Struct* oauth2_init(char* client, char* secret) {
	int input_strlen;
	oauth2Struct *os = malloc(sizeof(oauth2Struct));
	if (os == NULL)
		return NULL;
	input_strlen = strlen(client) +1;
	os->client_id = malloc(input_strlen *sizeof(char));
	strcpy(os->client_id, client);
	input_strlen = strlen(secret) + 1;
	os->client_secret = malloc(input_strlen);
	strcpy(os->client_secret, secret);
	os->error.error = OAUTH2_ERR_NO_ERROR;
	os->error.errorDescr = NULL;
	os->error.errorUrl = NULL;
	os->error.state = NULL;
	os->authCode = NULL;
	return os;

}

int main(int argc, char** argv) {

	oauth2Struct *os = oauth2_init("clientId", "clientSecret");
	oauth2ConfigureRedirectUrl(os, "http://abc.com");
	char *redirectUrl = oauth2RequestAuthCode(os, "https://google.com/authorize", "Location", "INIT");

	return 0;
}
