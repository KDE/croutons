// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

Library {
    name: "croutons"

    files: [
        "*.cpp",
        "*.h",
    ]

    readonly property bool isClang: qbs.toolchain.contains("clang")

    Export {
        readonly property bool isClang: qbs.toolchain.contains("clang")

        cpp.cxxFlags: isClang ? ["-fcoroutines-ts", "-stdlib=libc++"] : ["-fcoroutines"]
        cpp.cxxLanguageVersion: "c++20"
        cpp.includePaths: product.sourceDirectory
        cpp.rpaths: product.buildDirectory

        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "qml"] }
    }

    cpp.cxxFlags: isClang ? ["-fcoroutines-ts", "-stdlib=libc++"] : ["-fcoroutines"]
    cpp.cxxLanguageVersion: "c++20"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "qml"] }
}