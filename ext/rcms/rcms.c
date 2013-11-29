#include <ruby.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/err.h>


VALUE CMS = Qnil;
//VALUE CMSFc = Qnil;

void Init_rcms();
VALUE method_rcms_strcpy(VALUE self, VALUE payload);

void Init_rcms() {
	CMS = rb_define_module("CMS");
//	CMSFc = rb_define_module_under(CMS, "Fc");
	rb_define_singleton_method(CMS, "strcpy", method_rcms_strcpy, 1);
	rb_define_singleton_method(CMS, "verify", method_rcms_strcpy, 3);
}

VALUE method_rcms_strcpy(VALUE self, VALUE payload) { // Just for testing
	VALUE copy = Qnil;
	char* ccopy;

	asprintf(&ccopy, "%s", StringValuePtr(payload));
	copy = rb_str_new2(ccopy);

	free(ccopy);
	return copy;
}

VALUE verify(VALUE self, VALUE in, VALUE signer_pem, VALUE cacert_pem) {
	VALUE out = Qnil;



	return out;

}

