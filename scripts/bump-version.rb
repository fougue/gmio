#!/usr/bin/ruby

if not ARGV.empty? then
    version_str = ARGV.first
    version_array = ARGV.first.split('.')
    if version_array.size == 3 then
        major = version_array[0]
        minor = version_array[1]
        patch = version_array[2]
        puts "Major: #{major}"
        puts "Minor: #{minor}"
        puts "Patch: #{patch}"

        script_dir_name = File.expand_path(File.dirname(__FILE__))

        # CMakeLists.txt
        path_CMakeLists_txt = "#{script_dir_name}/../CMakeLists.txt"
        CMakeLists_txt = File.open(path_CMakeLists_txt, "r").read
        CMakeLists_txt.sub!(/(set\(GMIO_VERSION_MAJOR\s+)\d+/, "\\1#{major}")
        CMakeLists_txt.sub!(/(set\(GMIO_VERSION_MINOR\s+)\d+/, "\\1#{minor}")
        CMakeLists_txt.sub!(/(set\(GMIO_VERSION_PATCH\s+)\d+/, "\\1#{patch}")
        File.open(path_CMakeLists_txt, "w").write(CMakeLists_txt)
        puts "Bumped #{path_CMakeLists_txt}"

        # README.md
        path_README_md = "#{script_dir_name}/../README.md"
        README_md = File.open(path_README_md, "r").read
        README_md.sub!(
            /(img\.shields\.io\/badge\/version\-v)\d+\.\d+\.\d+/,
            "\\1#{version_str}")
        README_md.gsub!(
            /(www\.fougue\.pro\/docs\/gmio\/)\d+\.\d+/,
            "\\1#{major}.#{minor}")
        File.open(path_README_md, "w").write(README_md)
        puts "Bumped #{path_README_md}"

        # appveyor.yml
        path_appveyor_yml = "#{script_dir_name}/../appveyor.yml"
        appveyor_yml = File.open(path_appveyor_yml, "r").read
        appveyor_yml.sub!(
            /(version:\s+)\d+\.\d+(_build)/,
            "\\1#{major}.#{minor}\\2")
        File.open(path_appveyor_yml, "w").write(appveyor_yml)
        puts "Bumped #{path_appveyor_yml}"
    else
        puts "Error: wrong version format(maj.min.patch)"
    end
else
    puts "Error: no version argument"
end
