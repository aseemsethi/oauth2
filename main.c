/* 
 * Learnings from the code at https://github.com/slugonamission/OAuth2/blob/master/
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oauth2.h"

/*
 * GET /authorize?response_type=code&client_id=s6BhdRkqt3&state=xyz
 *         &redirect_uri=https%3A%2F%2Fclient%2Eexample%2Ecom%2Fcb HTTP/1.1
 *             Host: server.example.com
 *
 * URL: <server>?response_type=code&client_id=<client_id>&redirect_uri=<uri>&scope=<scope>&state=<state>
 */
char* oauth2RequestAuthCode(oauth2Struct *os, char* authServer, char* scope, char* state) {
	char* coreFmt = "%s?response_type=code&client_id=%s&redirect_uri=%s";
	char* scopeFmt = "&scope=%s";
	char* stateFmt = "&state=%s";

	int coreLen = snprintf(NULL, 0, (const char*)coreFmt, authServer, os->client_id, os->redirectUrl) + 1;
	int scopeLen = snprintf(NULL, 0, (const char*)scopeFmt, scope) + 1;
	int stateLen = snprintf(NULL, 0, (const char*)stateFmt, state) + 1;

	char* reqString = malloc((coreLen+scopeLen+stateLen - 3) *sizeof(char));
	sprintf(reqString, (const char*)coreFmt, authServer, os->client_id, os->redirectUrl);
	sprintf((char*)(reqString+(coreLen-1)), (const char*)scopeFmt, scope);
	sprintf((char*)(reqString+(coreLen-1)+(stateLen-1)), (const char*)stateFmt, state);
	return reqString;

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
	printf("\nRedirect URL: %s", redirectUrl);

	char code[255];
	printf("\nEnter the AuthToken: ");
	scanf("%s", code);

	// Now get the auth token using the Auth Code

	// Now use the Access Token to access the Resource on behalf of the user

	return 0;
}
