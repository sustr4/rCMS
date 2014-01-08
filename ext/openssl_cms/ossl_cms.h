#if !defined(_OSSL_CMS_H_)
#define _OSSL_CMS_H_

#if defined(__cplusplus)
extern "C" {
#endif

////////////////////////////
//
//	Ruby && OpenSSL stuff
//
////////////////////////////

#include "ossl.h"

//////////////////////////////
//	rCMS stuff
//////////////////////////////

extern VALUE cCMS;
extern VALUE eCMSError;

void Init_ossl_cms(void);

#if defined(__cplusplus)
}
#endif

#endif /* _OSSL_CMS_H_ */