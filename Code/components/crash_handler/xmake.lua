add_requires("sentry-native", { configs = { backend = "crashpad" } })

component("CrashHandler")
    add_packages("sentry-native")
