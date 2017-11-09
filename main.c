/* 
 * Learnings from the code at https://github.com/slugonamission/OAuth2/blob/master/
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oauth2.h"

/*
 *
 * (2) Authentication Code Request
 * ===============================
 *  https://accounts.google.com/o/oauth2/v2/auth?response_type=code&
 *  	client_id=681355735342-echlhpgf36phisuk13kcosd4em5q4ois.apps.googleusercontent.com&
 *  	redirect_uri=http://localhost/oauth2callback&
 *  	scope=https://www.googleapis.com/auth/userinfo.email&
 *  	state=INIT
 *
 * An error response:
 * 		https://localhost/auth?error=access_denied
 * An authorization code response:
 * 		http://localhost/oauth2callback?state=INIT&code=4/Zkx5zJ_Y72oY5k4G9pBfLOMwsAojaCsYlcyftq-y3FA#
 *
 * URL: <server>?response_type=code&client_id=<client_id>&redirect_uri=<uri>&scope=<scope>&state=<state>
 */
char* oauth2RequestAuthCode(oauth2Struct *os, char* authServer, char* scope, char* state) {
	char* coreFmt = "%s?response_type=code&client_id=%s&redirect_uri=%s";
	char* scopeFmt = "&scope=%s";
	char* stateFmt = "&state=%s";

	// snprintf returns the exact number of bytes minus the trailing null byte of the string, if the 1st param is NULL
	int coreLen = snprintf(NULL, 0, (const char*)coreFmt, authServer, os->client_id, os->redirectUrl);
	int scopeLen = snprintf(NULL, 0, (const char*)scopeFmt, scope);
	int stateLen = snprintf(NULL, 0, (const char*)stateFmt, state);

	char* reqString = malloc((coreLen+scopeLen+stateLen + 1) *sizeof(char));
	sprintf(reqString, (const char*)coreFmt, authServer, os->client_id, os->redirectUrl);
	sprintf((char*)(reqString+(coreLen)), (const char*)scopeFmt, scope);
	sprintf((char*)(reqString+(coreLen)+(scopeLen)), (const char*)stateFmt, state);
	return reqString;
}

/* 
 * (3) Access Token Request
 * ========================
 * POST /oauth2/v4/token HTTP/1.1
 * Host: www.googleapis.com
 * Content-Type: application/x-www-form-urlencoded
 *
 * code=4/P7q7W91a-oMsCeLvIaQm6bTrgtp7&
 * client_id=your_client_id&
 * client_secret=your_client_secret&
 * redirect_uri=https://oauth2.example.com/code&
 * grant_type=authorization_code
 *
 * Response
 * {
 *   "access_token":"1/fFAGRNJru1FTz70BzhT3Zg",
 *   "expires_in":3920,
 *   "token_type":"Bearer",
 *   "refresh_token":"1/xEoDL4iW3cxlI7yDbSRFYNG01kVKM2C-259HOF2aQbI"
 * }
 *
 */
int oauth2GetAccessToken(oauth2Struct *os, char* authServer, char* authCode, char* scope) {
	char *at_pos_start, *at_pos_end;
	int at_pos_len;
	int counter = 0;

	char *atFormat="grant_type=authorization_code&client_id=%s&client_secret=%s&code=%s&redirect_uri=%s";
	int atLen = snprintf(NULL, 0, (const char*)atFormat, os->client_id, os->client_secret, authCode, os->redirectUrl) + 1;
	char *url = malloc(sizeof(char)*atLen);
	sprintf(url, atFormat, os->client_id, os->client_secret, authCode, os->redirectUrl);
	printf("\n redirectUrl: %s", os->redirectUrl);
	printf("\n Making an Access Token request: %s", url);
	char *output = curl_make_request(authServer, url, 0 , 0);
	free(url);

	printf("\n%s", output);
	// Now, strip out the access token
	at_pos_start = strstr(output, "access_token");
	if (at_pos_start == NULL) {
		printf("\n Access Token not found");
		free(output); return 0;
	}
	printf("\nAccess Token Found: %s", at_pos_start);
	at_pos_start += 16; // skipping past access_token
	at_pos_end = at_pos_start;
	//while(*at_pos_end != '&' && *at_pos_end != '\0') {
	while(*at_pos_end != '\"') {
		++at_pos_end;
		if (counter++ > 250) {
			printf("\nAccess Token parse error");
			return 0;
		}
	}

	int at_os_len = at_pos_end - at_pos_start;
	char *at_code = malloc(sizeof(char)*at_os_len); 
	memcpy(at_code, at_pos_start, at_pos_end-at_pos_start);
	free(output);
	printf("\nAccess Token Len:%d:", at_os_len);
	for (counter=0; counter<at_os_len; counter++)
		printf("%c", at_code[counter]);
	printf("\n");
	oauth2ConfigureAccessToken(os, at_code, at_os_len);
	return 1;
}

/*
 * (4) Resource Access
 * ===================
 * GET /drive/v2/files HTTP/1.1
 * Authorization: Bearer <access_token>
 * Host: www.googleapis.com/
 * Here is a call to the same API for the authenticated user using the access_token query string parameter:
 * GET https://www.googleapis.com/drive/v2/files?access_token=<access_token>
 *
 * Using curl:
 * $ curl -H "Authorization: Bearer ya29.Glz_BAkVERUNzctee-o520khCJrt4TUZfABynij3XM9wATyHeiZibxvJjhr1CuWkKLKuNb-dZa-z9Gl9EAva0bZsLiuVwNWRDn3nGoAD2zuvh8S7X_ERpD20Fs1rKA" https://www.googleapis.com/oauth2/v1/userinfo
 * {
 *  "id": "101605605917347842452",
 *  "email": "aseemsethi70@gmail.com",
 *  "verified_email": true,
 *  "name": "aseem sethi",
 *  "given_name": "aseem",
 *  "family_name": "sethi",
 *  "link": "https://plus.google.com/101605605917347842452",
 *  "picture": "https://lh4.googleusercontent.com/-QfvMhNsei1w/AAAAAAAAAAI/AAAAAAAAAAA/L-lUGpurJi0/photo.jpg",
 *  "gender": "male"
 *  }
 *
 * API endpoint - "https://www.googleapis.com/oauth2/v1/userinfo?alt=json");
 *
 */
int oauth2ResourceRequest (oauth2Struct* os, char* uri) {
	char* fmt = "Authorization: Bearer ";
	char* reqString = malloc((strlen(fmt)+os->accessTokenLen+1)*sizeof(char));
	sprintf(reqString, (const char*)fmt);
	memcpy(reqString+strlen(fmt), os->accessToken, os->accessTokenLen);

	reqString[strlen(fmt)+os->accessTokenLen] = '\0';

	printf("\nSending oauth2 Resource Request with Auth Hdr:%s", reqString);
	char *output = curl_make_request("https://www.googleapis.com/oauth2/v1/userinfo?alt=json", 0, reqString, 1);
	printf("\nResource Response: %s", output);
	printf("\n");
	return 1;
}


int oauth2ConfigureAccessToken(oauth2Struct *os, char *at, int len) {
	if (at == NULL) {
		printf("\n Error: Access Token not retrieved");
		return 0;
	}
	os->accessToken = malloc(len * sizeof(char));
	memcpy(os->accessToken, at, len);
	os->accessTokenLen = len;
	return 1;
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

/*
 * (1) Client Registration
 * ===================
 * goto webpage - https://developers.google.com/identity/protocols/OAuth2WebServer
 * and setup a project as defined in https://console.developers.google.com/apis/credentials
 *
 * Google Example Project used:
 * WebApp Url:    http://aseemsethi.example.com
 * redirectUrl:   http://aseemsethi.example.com/oauth2
 * "client_id":"681355735342-echlhpgf36phisuk13kcosd4em5q4ois.apps.googleusercontent.com",
 * "project_id":"round-music-88012",
 * "auth_uri":"https://accounts.google.com/o/oauth2/auth",
 * "token_uri":"https://accounts.google.com/o/oauth2/token",
 * "auth_provider_x509_cert_url":"https://www.googleapis.com/oauth2/v1/certs",
 * "client_secret":"tWDvRWNQLtL-erzM8mQva_K7",
 * "redirect_uris":["http://aseemsethi.example.com/oauth2"],
 * "javascript_origins":["http://aseemsethi.example.com"]}}
 */
int main(int argc, char** argv) {
	int status;
	char msg1[255], msg2[255];
	char client[255], secret[255];

	printf("\nOauth2 in Action..........");
	printf("\nUser goes to a web site, and clicks on \'Login via Google\'");
	printf("\nThis sends a GET request to Google requesting for an auth code");
	printf("\nAuth Code params:");

	printf("\n	Enter client id (clientId):");
	fgets(msg1, 254, stdin);
	if (msg1[0] == '\n') 
		strcpy(client, "681355735342-echlhpgf36phisuk13kcosd4em5q4ois.apps.googleusercontent.com");
	else {
		// remove the trailing \n put by fgets
		strtok(msg1, "\n");
		strcpy(client, msg1);
	}

	printf("\n	Enter client secret(clientSecret):");
	fgets(msg2, 254, stdin);
	if (msg2[0] == '\n') 
		strcpy(secret, "tWDvRWNQLtL-erzM8mQva_K7");
	else {
		// remove the trailing \n put by fgets
		strtok(msg2, "\n");
		strcpy(secret, msg2);
	}

	oauth2Struct *os = oauth2_init(client, secret);
	oauth2ConfigureRedirectUrl(os, "http://localhost/oauth2callback");
	// call with - auth server, scope, state
	// Google's OAuth 2.0 endpoint is at https://accounts.google.com/o/oauth2/v2/auth
	char *redirectUrl = oauth2RequestAuthCode(os, "https://accounts.google.com/o/oauth2/v2/auth",
							"https://www.googleapis.com/auth/userinfo.email", "INIT");
	printf("\n\nPlease go to this url in browser: \n%s\n", redirectUrl);

	char code[255];
	printf("\nFrom the Response, pick up the AuthToken and put it here: ");
	scanf("%s", code);

	// Now get the auth token using the Auth Code from https://www.googleapis.com/oauth2/v4/token
	status = oauth2GetAccessToken(os, "https://www.googleapis.com/oauth2/v4/token",
									code, "https://www.googleapis.com/auth/userinfo.email");
	if (status == 0) return;
	

	// Now use the Access Token to access the Resource on behalf of the user
	status = oauth2ResourceRequest(os, "https://www.googleapis.com/oauth2/v1/userinfo?alt=json");
	while(1) {
		sleep(5);
	}

	return 0;
}
