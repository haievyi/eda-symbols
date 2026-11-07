cmake_minimum_required(VERSION 3.15)

add_executable(test_symbols
    test_symbols.cpp
)
target_link_libraries(test_symbols PRIVATE eda-symbols)

add_executable(export_symbol_images
    export_symbol_images.cpp
)
target_link_libraries(export_symbol_images PRIVATE eda-symbols)

add_test(NAME UnitTests COMMAND test_symbols)
