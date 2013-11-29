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

void Init_rcms() {
	CMS = rb_define_module("CMS");
	rb_define_singleton_method(CMS, "strcpy", method_rcms_strcpy, 1);
	rb_define_singleton_method(CMS, "verify", method_rcms_verify, 3);
}

VALUE method_rcms_strcpy(VALUE self, VALUE payload) { // Just for testing
	VALUE copy = Qnil;
	char* ccopy;

	ccopy = strdup(StringValuePtr(payload));
	copy = rb_str_new2(ccopy);

	fprintf(stderr, "\n\n*** CMS.strcpy\n\n");

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

	int len;

	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();

	/* Set up trusted CA certificate store */
	st = X509_STORE_new();

	/* Read in CA certificate */

	cacert_str = StringValuePtr(cacert_pem);
	BIO_write(tbio, cacert_str, strlen(cacert_str));
//	free(cacert_str)

	if (!tbio) goto err;


	cacert = PEM_read_bio_X509(tbio, NULL, 0, NULL);
	if (!cacert) goto err;

	// Add CA Cert into store
	if (!X509_STORE_add_cert(st, cacert)) goto err;



	// Read in signing certificate
	signer_str = StringValuePtr(signer_pem);
	BIO_write(sbio, signer_str, strlen(signer_str));
	signing = PEM_read_bio_X509(sbio, NULL, 0, NULL);
//	free(signer_str)
	if (!sbio) goto err;

	// Push signing certificate to stack
	if(!sk_X509_push(sk, signing))
		goto err;



	// Open message to verify
	in_str = StringValuePtr(in_pem);
	BIO_write(in_bio, in_str, strlen(in_str));
	if (!in_bio) goto err;

	
	// parse message




	cms = PEM_read_bio_CMS(in_bio, NULL, NULL, NULL);
	if (!cms) goto err;

	// verify
	if (!CMS_verify(cms, sk, st, NULL, out_bio, 0)) {
		fprintf(stderr, "Verification Failure\n");
		goto err;
	}

	char *buf, *tmpstr;
	for ( ; ; ) {
		buf = calloc(sizeof(char), 1024 + 1);
		len = BIO_read(out_bio, buf, 1024);
		asprintf(&tmpstr, "%s%s", out_str ? out_str : "", buf);
		free(out_str); out_str = tmpstr;
		if (len <= 0) break;
	}
	out = rb_str_new2(out_str);
	

	err:

	if (!out) {
		fprintf(stderr, "Error Verifying Data\n");
		ERR_print_errors_fp(stderr);
	}

	if (cms) CMS_ContentInfo_free(cms);

	if (cacert) X509_free(cacert);

	if (in_bio) BIO_free(in_bio);
	if (out_bio) BIO_free(out_bio);
	if (tbio) BIO_free(tbio);


	return out;

}

