cmake_minimum_required(VERSION 3.10)
project(CSFMLProject C)

set(CMAKE_C_STANDARD 17)

# Caminho para CSFML
include_directories("C:/CSFML/include")
link_directories("C:/CSFML/lib/gcc")

# Adicione os arquivos do projeto
add_executable(main
    main.c
    interfaces/pauseMenu.c
)

# Linkar bibliotecas CSFML e outras do sistema
target_link_libraries(main
    csfml-graphics
    csfml-window
    csfml-system
    csfml-audio
    winmm
    gdi32
    opengl32
)

# Suprimir warnings específicos (opcional)
add_compile_options(-Wno-deprecated-declarations)