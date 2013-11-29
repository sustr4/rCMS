#include "ossl_cms.h"

#define WrapCMS(klass, obj, cms) do { \
    if (!(cms)) { \
    ossl_raise(rb_eRuntimeError, "CMS wasn't initialized."); \
    } \
    (obj) = Data_Wrap_Struct((klass), 0, CMS_ContentInfo_free, (cms)); \
} while (0)
#define GetCMS(obj, cms) do { \
    Data_Get_Struct((obj), CMS, (cms)); \
    if (!(cms)) { \
    ossl_raise(rb_eRuntimeError, "CMS wasn't initialized."); \
    } \
} while (0)
#define SafeGetCMS(obj, cms) do { \
    OSSL_Check_Kind((obj), cCMS); \
    GetCMS((obj), (cms)); \
} while (0)

#define numberof(ary) (int)(sizeof(ary)/sizeof((ary)[0]))

#define ossl_cms_set_data(o,v)       rb_iv_set((o), "@data", (v))
#define ossl_cms_get_data(o)         rb_iv_get((o), "@data")
#define ossl_cms_set_err_string(o,v) rb_iv_set((o), "@error_string", (v))
#define ossl_cms_get_err_string(o)   rb_iv_get((o), "@error_string")

VALUE cCMS;
VALUE eCMSError;

static VALUE
ossl_cms_s_read_cms(VALUE klass, VALUE arg)
{
    BIO *in, *out;
    CMS_ContentInfo *cms;
    VALUE ret, data;

    in = ossl_obj2bio(arg);
    out = NULL;
    cms = PEM_read_bio_CMS(in, &out, NULL, NULL)

    BIO_free(in);

    if(!cms) ossl_raise(eCMSError, NULL);

    data = out ? ossl_membio2str(out) : Qnil;

    WrapCMS(cCMS, ret, cms);
    ossl_cms_set_data(ret, data);
    ossl_cms_set_err_string(ret, Qnil);

    return ret;
}

static VALUE
ossl_cms_alloc(VALUE klass)
{
    CMS *cms;
    VALUE obj;

    if (!(cms = CMS_ContentInfo_new())) {
        ossl_raise(eCMSError, NULL);
    }
    WrapCMS(klass, obj, cms);

    return obj;
}

static VALUE
ossl_cms_initialize(int argc, VALUE *argv, VALUE self)
{
    CMS_ContentInfo *c, *cms = DATA_PTR(self);
    BIO *in;
    VALUE arg;

    if(rb_scan_args(argc, argv, "01", &arg) == 0)
    return self;

    arg = ossl_to_der_if_possible(arg);
    in = ossl_obj2bio(arg);

    c = PEM_read_bio_CMS(in, &cms, NULL, NULL);
    if (!c) {
        OSSL_BIO_reset(in);
        c = d2i_CMS_bio(in, &cms);

        if (!c) {
            BIO_free(in);
            CMS_ContentInfo_free(cms);
            DATA_PTR(self) = NULL;
            ossl_raise(rb_eArgError, "Could not parse the CMS");
        }
    }

    DATA_PTR(self) = cms;
    BIO_free(in);
    ossl_cms_set_data(self, Qnil);
    ossl_cms_set_err_string(self, Qnil);

    return self;
}

static VALUE
ossl_cms_copy(VALUE self, VALUE other)
{
    CMS_ContentInfo *a, *b, *cms;

    rb_check_frozen(self);
    if (self == other) return self;

    GetCMS(self, a);
    SafeGetCMS(other, b);

    cms = CMS_ContentInfo_dup(b);
    if (!cms) {
        ossl_raise(eCMSError, NULL);
    }
    DATA_PTR(self) = cms;
    CMS_ContentInfo_free(a);

    return self;
}

static VALUE
ossl_cms_verify(int argc, VALUE *argv, VALUE self)
{
    VALUE certs, store, indata, flags;
    STACK_OF(X509) *x509s;
    X509_STORE *x509st;
    int flg, ok, status = 0;
    BIO *in, *out;
    CMS_ContentInfo *cms;
    VALUE data;
    const char *msg;

    rb_scan_args(argc, argv, "22", &certs, &store, &indata, &flags);
    flg = NIL_P(flags) ? 0 : NUM2INT(flags);

    if(NIL_P(indata)) indata = ossl_cms_get_data(self);
    in = NIL_P(indata) ? NULL : ossl_obj2bio(indata);

    if(NIL_P(certs)) x509s = NULL;
    else{
        x509s = ossl_protect_x509_ary2sk(certs, &status);
        if(status){
            BIO_free(in);
            rb_jump_tag(status);
        }
    }

    x509st = GetX509StorePtr(store);
    GetCMS(self, cms);

    if(!(out = BIO_new(BIO_s_mem()))){
        BIO_free(in);
        sk_X509_pop_free(x509s, X509_free);
        ossl_raise(eCMSError, NULL);
    }

    ok = CMS_verify(cms, x509s, x509st, in, out, flg);

    BIO_free(in);
    if (ok < 0) ossl_raise(eCMSError, NULL);

    msg = ERR_reason_error_string(ERR_get_error());
    ossl_cms_set_err_string(self, msg ? rb_str_new2(msg) : Qnil);
    ERR_clear_error();

    data = ossl_membio2str(out);
    ossl_cms_set_data(self, data);
    sk_X509_pop_free(x509s, X509_free);

    return (ok == 1) ? Qtrue : Qfalse;
}

static VALUE
ossl_cms_to_der(VALUE self)
{
    CMS_ContentInfo *cms;
    VALUE str;
    long len;
    unsigned char *p;

    GetCMS(self, cms);
    if((len = i2d_CMS(cms, NULL)) <= 0)
        ossl_raise(eCMSError, NULL);

    str = rb_str_new(0, len);
    p = (unsigned char *)RSTRING_PTR(str);

    if(i2d_CMS(cms, &p) <= 0)
        ossl_raise(eCMSError, NULL);

    ossl_str_adjust(str, p);

    return str;
}

static VALUE
ossl_cms_to_pem(VALUE self)
{
    CMS_ContentInfo *cms;
    BIO *out;
    VALUE str;

    GetCMS(self, cms);
    if (!(out = BIO_new(BIO_s_mem()))) {
        ossl_raise(eCMSError, NULL);
    }
    if (!PEM_write_bio_CMS(out, cms)) {
        BIO_free(out);
        ossl_raise(eCMSError, NULL);
    }
    str = ossl_membio2str(out);

    return str;
}

void
Init_ossl_cms()
{
    cCMS = rb_define_class_under(mOSSL, "CMS", rb_cObject);
    eCMSError = rb_define_class_under(cCMS, "CMSError", rb_eStandardError);

    rb_define_singleton_method(cCMS, "read_cms", ossl_cms_s_read_cms, 1);

    rb_attr(cCMS, rb_intern("data"), 1, 0, Qfalse);
    rb_attr(cCMS, rb_intern("error_string"), 1, 1, Qfalse);

    rb_define_alloc_func(cCMS, ossl_cms_alloc);
    rb_define_copy_func(cCMS, ossl_cms_copy);

    rb_define_method(cCMS, "initialize", ossl_cms_initialize, -1);
    rb_define_method(cCMS, "verify", ossl_cms_verify, -1);
    rb_define_method(cCMS, "to_pem", ossl_cms_to_pem, 0);
    rb_define_alias(cCMS,  "to_s", "to_pem");
    rb_define_method(cCMS, "to_der", ossl_cms_to_der, 0);
}