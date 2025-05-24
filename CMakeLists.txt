cmake_minimum_required(VERSION 3.10)
project(CSFMLProject C)

set(CMAKE_C_STANDARD 17)


include_directories("C:/CSFML/include")
link_directories("C:/CSFML/lib/gcc")


add_executable(main
    main.c
    
)


target_link_libraries(main
    csfml-graphics
    csfml-window
    csfml-system
    csfml-audio
    winmm
    gdi32
    opengl32
)


add_compile_options(-Wno-deprecated-declarations)