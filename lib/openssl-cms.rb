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

require 'openssl-cms.so'

require 'openssl-cms/bn'
require 'openssl-cms/cipher'
require 'openssl-cms/config'
require 'openssl-cms/digest'
require 'openssl-cms/ssl-internal'
require 'openssl-cms/x509-internal'

