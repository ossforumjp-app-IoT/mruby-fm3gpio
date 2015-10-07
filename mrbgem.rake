MRuby::Gem::Specification.new('mruby-fm3gpio') do |spec|
  spec.license = 'MIT'
  spec.author  = 'lsi-dev'
  spec.summary = 'fm3 gpio class'

  spec.cc.include_paths << '#{build.root}/src'
end
