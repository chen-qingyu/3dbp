set_project("3dbp")
set_version("0.1.0")
set_license("MIT")
set_languages("cxx20")
set_encodings("utf-8")

add_rules("mode.debug", "mode.release")
add_requires("nlohmann_json", "spdlog", "json-schema-validator", "catch2")

target("3dbp")
    set_kind("binary")
    set_rundir(".")
    add_packages("nlohmann_json", "spdlog", "json-schema-validator")
    add_files("sources/*.cpp")

    after_run(function (target)
        local args = import("core.base.option").get("arguments")
        os.execv("py", {"draw.py", "./result/result-" .. path.filename(args[1])})
    end)

target("test")
    set_kind("binary")
    set_rundir(".")
    add_packages("nlohmann_json", "spdlog", "json-schema-validator", "catch2")
    add_files("tests/*.cpp")


target("bench")
    set_kind("binary")
    set_rundir(".")
    add_packages("nlohmann_json", "spdlog", "catch2")
    add_files("benches/*.cpp")

    before_run(function (target)
        os.execv("py", {"data/br_txt_to_json.py"})
    end)
