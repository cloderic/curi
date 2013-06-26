add_library(curi curi.c curi.h)

if(CMAKE_COMPILER_IS_GNUCC)
    set_property( TARGET curi APPEND_STRING PROPERTY COMPILE_FLAGS "-Wall -Werror")
elseif(MSVC)
    set_property( TARGET curi APPEND_STRING PROPERTY COMPILE_FLAGS "/W4 /WX" )
endif()

install(FILES curi.h curi.c DESTINATION ./src)