cmake_minimum_required (VERSION 3.0)

project(main)

# добавляем директории для поиска файлов
include_directories(./)

# добавляем подпроект
add_subdirectory(A)

# добавляем испольняемый файл
add_executable(main B/main.cpp)

# линкуем библиотику
target_link_libraries(main INTERFACE lib)

# проверяем присутсвие интерпретатора
find_package( PythonInterp 2.7 REQUIRED )

# выполняем сборку header файла
add_custom_target(run ALL
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/C/preparing.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/C/
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/C/preparing.py
        COMMENT "Preparing header index.h"
        )

# создаем dependency на основой исполняемый файл
add_dependencies(main run)

