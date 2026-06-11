add_requires("cjson", "libcurl", "mongoose")

target("cspotify")
    add_files("src/spotify/*.c", "src/*.c")
    add_includedirs("include")
    add_packages("cjson", "libcurl", "mongoose")
    set_license("GPL-2.0")
