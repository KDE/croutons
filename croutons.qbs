Project {
    references: [
        "lib/croutons.qbs",
        "test/test.qbs",
    ]

    AutotestRunner { name: "croutons-tests" }
}