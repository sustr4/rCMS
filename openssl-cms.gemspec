Gem::Specification.new do |gem|
  gem.name = "openssl-cms"
  gem.version = "0.0.1"
  gem.summary = "OpenSSL with CMS functions for Ruby"
  gem.author = "Zdenek Sustr, Boris Parak"
  gem.homepage = 'https://github.com/arax/openssl-cms'
  gem.license = 'See LICENSE'
  
  gem.files = Dir.glob("lib/**/*.rb")
  gem.require_path = 'lib'
  
  gem.extensions << "ext/openssl-cms/extconf.rb"
  
  gem.required_ruby_version = '>= 1.9.3'
end

