set(SOURCES
    main.cpp
    ils.h
    ils.cpp
    gls.h
    gls.cpp
    local_search.h
    local_search.cpp
    utils.h
    utils.cpp
    problem.h
    problem.cpp
    heuristics.h
    heuristics.cpp
    procedures.h
    procedures.cpp
)

find_package(Threads REQUIRED)

add_executable(SlashchininLab1 ${SOURCES})

target_link_libraries(SlashchininLab1 ${CMAKE_THREAD_LIBS_INIT})
