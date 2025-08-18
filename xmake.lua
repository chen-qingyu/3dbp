set_project("3dbp")
set_version("0.1.0")
set_license("MIT")
set_languages("cxx20")

add_rules("mode.debug", "mode.release")
add_requires("nlohmann_json", "spdlog")

target("3dbp")
    set_kind("binary")
    set_rundir(".")
    add_packages("nlohmann_json", "spdlog")
    add_files("sources/*.cpp")
