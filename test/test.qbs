// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

QtApplication {
    name: "croutons-test"

    files: ["*.cpp", "*.h"]
    type: ["application", "autotest"]

    Depends { name: "croutons" }
    Depends { name: "Qt"; submodules: ["gui"] }
}
