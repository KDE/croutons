// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

import qbs.FileInfo

Library {
    name: "croutons"
    version: "1.1-dev"

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

        prefixMapping: [{
            prefix: product.sourceDirectory,
            replacement: FileInfo.joinPaths(qbs.installPrefix, "include")
        }, {
            prefix: product.buildDirectory,
            replacement: FileInfo.joinPaths(qbs.installPrefix, "lib64")
        }]

        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: ["core", "qml"] }
    }

    cpp.cxxFlags: isClang ? ["-fcoroutines-ts", "-stdlib=libc++"] : ["-fcoroutines"]
    cpp.cxxLanguageVersion: "c++20"

    install: true
    installDir: "lib64"

    Group {
        fileTagsFilter: ["Exporter.pkgconfig.pc"]
        qbs.install: true
        qbs.installDir: "lib64/pkgconfig"
    }
    Group {
        fileTagsFilter: ["Exporter.qbs.module"]
        qbs.install: true
        qbs.installDir: "share/qbs/modules/croutons"
    }

    Depends { name: "cpp" }
    Depends { name: "Exporter.pkgconfig" }
    Depends { name: "Exporter.qbs" }
    Depends { name: "Qt"; submodules: ["core", "qml"] }
}
