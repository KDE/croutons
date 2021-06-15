QtApplication {
	files: ["*.cpp", "*.h"]
	cpp.cxxFlags: ["-fcoroutines"]
	cpp.cxxLanguageVersion: "c++20"
	Depends { name: "Qt"; submodules: ["network", "qml", "gui"] }
}
