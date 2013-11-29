#include <ruby.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>

VALUE CMS = Qnil;
//VALUE CMSFc = Qnil;

void Init_rcms();
VALUE method_rcms_strcpy(VALUE self, VALUE payload);
VALUE method_rcms_verify(VALUE self, VALUE in_pem, VALUE signer_pem, VALUE cacert_pem);
VALUE exception_VerificationError;
char *BIO_to_str(BIO *inBIO);
char *ERR_to_str();

void Init_rcms() {
	CMS = rb_define_module("CMS");
	rb_define_singleton_method(CMS, "strcpy", method_rcms_strcpy, 1);
	rb_define_singleton_method(CMS, "verify", method_rcms_verify, 3);

	exception_VerificationError = rb_define_class_under(CMS, "Error", rb_eStandardError);
}

VALUE method_rcms_strcpy(VALUE self, VALUE payload) { // Just for testing
	VALUE copy = Qnil;
	char* ccopy;

	ccopy = strdup(StringValuePtr(payload));
	copy = rb_str_new2(ccopy);

	free(ccopy);
	return copy;
}

VALUE method_rcms_verify(VALUE self, VALUE in_pem, VALUE signer_pem, VALUE cacert_pem) {
	VALUE out = Qnil;

	BIO *in_bio = BIO_new(BIO_s_mem()), *out_bio = BIO_new(BIO_s_mem()), *tbio = BIO_new(BIO_s_mem()), *sbio = BIO_new(BIO_s_mem());
	X509_STORE *st = NULL;
	X509 *cacert = NULL, *signing = NULL;
	CMS_ContentInfo *cms = NULL;
	STACK_OF(X509) *sk = sk_X509_new_null();

	char *cacert_str = NULL, *signer_str = NULL, *in_str = NULL, *out_str = NULL;

	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	/* Set up trusted CA certificate store */
	st = X509_STORE_new();

	/* Read in CA certificate */

	cacert_str = StringValuePtr(cacert_pem);
	BIO_write(tbio, cacert_str, strlen(cacert_str));

	if (!tbio) rb_raise(exception_VerificationError, "Error getting CA certificates: %s", ERR_to_str());


	cacert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
	if (!cacert) rb_raise(exception_VerificationError, "Error interpretting CA cert: %s", ERR_to_str());

	// Add CA Cert into store
	if (!X509_STORE_add_cert(st, cacert)) rb_raise(exception_VerificationError, "Error adding CA to store: %s", ERR_to_str());



	// Read in signing certificate
	signer_str = StringValuePtr(signer_pem);
	BIO_write(sbio, signer_str, strlen(signer_str));
	signing = PEM_read_bio_X509(sbio, NULL, 0, NULL);
	if (!sbio) rb_raise(exception_VerificationError, "Error getting signer certificate: %s", ERR_to_str());

	// Push signing certificate to stack
	if(!sk_X509_push(sk, signing)) rb_raise(exception_VerificationError, "Error creating signer certificate stack: %s", ERR_to_str());



	// Open message to verify
	in_str = StringValuePtr(in_pem);
	BIO_write(in_bio, in_str, strlen(in_str));
	if (!in_bio) rb_raise(exception_VerificationError, "Error getting input message: %s", ERR_to_str());

	
	// parse message




	cms = PEM_read_bio_CMS(in_bio, NULL, NULL, NULL);
	if (!cms) rb_raise(exception_VerificationError, "Error reading CMS: %s", ERR_to_str());

	// verify
	if (!CMS_verify(cms, sk, st, NULL, out_bio, 0)) rb_raise(exception_VerificationError, "Error verifying CMS message: %s", ERR_to_str());

	out_str = BIO_to_str(out_bio);
	out = rb_str_new2(out_str);
	

	if (!out_str) {
		rb_raise(exception_VerificationError, "%s", ERR_to_str());
	}

	// Cleanup
	free(cacert_str);
	free(signer_str);
	free(in_str);
	free(out_str);
	if (cms) CMS_ContentInfo_free(cms);

	if (cacert) X509_free(cacert);

	if (in_bio) BIO_free(in_bio);
	if (out_bio) BIO_free(out_bio);
	if (tbio) BIO_free(tbio);


	return out;

}


char *BIO_to_str(BIO *inBIO) {
        char *buf, *tmpstr, *ret = NULL;
	int len;

        for ( ; ; ) {
                buf = calloc(sizeof(char), 1024 + 1);
                len = BIO_read(inBIO, buf, 1024);
                if (len <= 0) break;
                else {
			asprintf(&tmpstr, "%s%s", ret ? ret : "", buf);
	                free(ret); ret = tmpstr;
		}
        }

	return ret;
}

char *ERR_to_str() {
	BIO *errmsg = BIO_new(BIO_s_mem());

	ERR_print_errors(errmsg);
	return BIO_to_str(errmsg);
}
