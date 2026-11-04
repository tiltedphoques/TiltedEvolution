
target("TPTests")
    set_kind("binary")
    set_group("Tests")
    add_includedirs(
        ".", "../encoding")
    add_headerfiles("**.h")
    add_files("*.cpp")
    -- Has no game dependencies, so the client's remote playback timing can be tested here
    add_files("../client/Systems/PlaybackClock.cpp")
    -- Voice chat's noise gate, falloff and jitter buffer, also game independent
    add_files("../client/Systems/VoiceDsp.cpp")
    add_deps("SkyrimEncoding")
    add_packages(
        "tiltedcore",
        "hopscotch-map",
        "catch2",
        "mimalloc",
        "glm")
