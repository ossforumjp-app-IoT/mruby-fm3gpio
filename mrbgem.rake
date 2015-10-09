MRuby::Gem::Specification.new('mruby-fm3gpio') do |spec|
  spec.license = 'Apache 2.0'
  spec.author  = 'lsi-dev'
  spec.summary = 'fm3 gpio class'

  spec.cc.include_paths << '#{build.root}/src'
end
