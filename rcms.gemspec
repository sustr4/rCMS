Gem::Specification.new do |s|
  s.name    = "rcms"
  s.version = "0.0.1"
  s.summary = "CMS functions wrapper for Ruby"
  s.author  = "Zdenek Sustr, Boris Parak"
  
  s.files = Dir.glob("ext/**/*.{c,rb}") +
            Dir.glob("lib/**/*.rb")
  
  s.extensions << "ext/rcms/rcms.rb"
  
  s.add_development_dependency "rake-compiler"
end

