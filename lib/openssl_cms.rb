=begin
= $RCSfile$ -- Loader for all OpenSSL C-space and Ruby-space definitions

= Info
  'OpenSSL for Ruby 2' project
  Copyright (C) 2002  Michal Rokos <m.rokos@sh.cvut.cz>
  All rights reserved.

= Licence
  This program is licenced under the same licence as Ruby.
  (See the file 'LICENCE'.)

= Version
  $Id$
=end

if Object.const_defined? :OpenSSL
  Object.send(:remove_const, :OpenSSL)
end

require 'openssl_cms.so'

require 'openssl_cms/bn'
require 'openssl_cms/cipher'
require 'openssl_cms/config'
require 'openssl_cms/digest'
require 'openssl_cms/ssl-internal'
require 'openssl_cms/x509-internal'

