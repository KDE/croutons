QtApplication {
    files: ["*.cpp", "*.h"]
    type: ["application", "autotest"]

    Depends { name: "croutons" }
    Depends { name: "Qt"; submodules: ["gui"] }
}
