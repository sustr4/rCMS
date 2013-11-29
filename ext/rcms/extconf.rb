require 'mkmf'
extension_name = 'rcms'
dir_config(extension_name)

pkg_config("openssl")

create_makefile(extension_name)
