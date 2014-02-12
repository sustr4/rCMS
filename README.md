OpenSSL-CMS for Ruby
====================
**At the moment, only token validation is supported.**

## CMS token validation
~~~
# load modified version of OSSL4R
# this will replace the original library
require 'openssl_cms'

# CMS token
token = "-----BEGIN CMS-----\n#{TOKEN_HERE}\n-----END CMS-----\n"

cms = OpenSSL::CMS.read_cms(token)
# or
#cms = OpenSSL::CMS.new(token)

# establish the chain of trust
ca_crt = OpenSSL::X509::Certificate.new(File.read('ca.cert'))
store = OpenSSL::X509::Store.new
store.add_cert ca_crt

# provide the signing certificate
crt = OpenSSL::X509::Certificate.new(File.read('signing.cert'))

# perform validation
verified = cms.verify([crt], store, nil, nil)

# check the result
if verified
  cms.data
else
  raise "Token verification failed!"
end
~~~
