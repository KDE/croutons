// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

Project {
    references: [
        "lib/croutons.qbs",
        "test/test.qbs",
    ]

    AutotestRunner { name: "croutons-tests" }
}