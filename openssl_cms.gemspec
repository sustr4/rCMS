Gem::Specification.new do |gem|
  gem.name = "openssl_cms_2_2"
  gem.version = "0.0.3"
  gem.summary = "OpenSSL with CMS functions for Ruby 2.2"
  gem.authors = ["Zdenek Sustr", "Boris Parak", "František Dvořák"]
  gem.homepage = 'https://github.com/arax/openssl-cms'
  gem.license = 'https://github.com/arax/openssl-cms/blob/master/LICENSE'
  gem.description = "OpenSSL with CMS functions for Ruby 2.2"
  gem.email = ['parak@cesnet.cz']
  
  gem.files = `git ls-files`.split("\n")
  gem.require_paths = ['lib']
  
  gem.extensions << "ext/openssl_cms/extconf.rb"
  
  gem.required_ruby_version = '~> 2.2.0'
end

