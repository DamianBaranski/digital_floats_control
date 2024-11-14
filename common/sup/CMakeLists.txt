target_include_directories(common_lib PUBLIC 
.
)

target_sources(common_lib PRIVATE
base64.cpp
)

add_subdirectory(utest)