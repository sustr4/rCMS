Gem::Specification.new do |gem|
  gem.name = "openssl_cms_2_0_0"
  gem.version = "0.0.2"
  gem.summary = "OpenSSL with CMS functions for Ruby 2.0.0"
  gem.authors = ["Zdenek Sustr", "Boris Parak"]
  gem.homepage = 'https://github.com/arax/openssl-cms'
  gem.license = 'See LICENSE at https://github.com/arax/openssl-cms/blob/master/LICENSE'
  gem.description = "OpenSSL with CMS functions for Ruby 2.0.0"
  gem.email = ['parak@cesnet.cz']
  
  gem.files = `git ls-files`.split("\n")
  gem.require_paths = ['lib']
  
  gem.extensions << "ext/openssl_cms/extconf.rb"
  
  gem.required_ruby_version = '~> 2.0.0'
end

