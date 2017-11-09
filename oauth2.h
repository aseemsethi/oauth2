#ifndef __OAUTH2_H__
#define __OAUTH2_H_

/*
#ifndef NULL
	#define NULL 0
#endif
*/

typedef enum {
	OAUTH2_RESPONSE_CODE = 0,
	OAUTH2_RESPONSE_TOKEN,
	OAUTH2_RESPONSE_TOKEN_AND_CODE
} responseCode;

typedef enum {
	OAUTH2_ERR_NO_ERROR = 0,
	OAUTH2_ERR_INVALID_REQUEST,
    OAUTH2_ERR_INVALID_CLIENT,
    OAUTH2_ERR_UNAUTHORIZED_CLIENT,
    OAUTH2_ERR_REDIRECT_URI_MISMATCH,
    OAUTH2_ERR_ACCESS_DENIED,
    OAUTH2_ERR_UNSUPPORTED_RESPONSE_TYPE,
    OAUTH2_ERR_INVALID_SCOPE,
    OAUTH2_ERR_INVALID_GRANT,
    OAUTH2_ERR_UNSUPPORTED_GRANT_TYPE,
} errorCode;

typedef struct _oauth2ErrStruct {
	errorCode			error;
	char*				errorDescr;
	char*				errorUrl;
	char*				state;
} oauth2Err;

typedef struct _oauth2Struct {
	char*		client_id;
	char*		client_secret;
	char*		redirectUrl;
	char*		authCode;
	char*		accessToken;
	int 		accessTokenLen;
	oauth2Err	error;
} oauth2Struct;

#endif
