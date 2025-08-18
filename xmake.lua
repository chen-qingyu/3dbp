set_project("3dbp")
set_version("0.1.0")
set_license("MIT")
set_languages("cxx20")

add_rules("mode.debug", "mode.release")

target("3dbp")
    set_kind("binary")
    add_files("sources/*.cpp")
