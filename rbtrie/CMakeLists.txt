add_executable(rbtrie "rbtrie.cpp" "rbtrie.h" "rbtrieRB.h")

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET rbtrie PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(rbtrie PRIVATE uni-algo::uni-algo)

add_custom_command(TARGET rbtrie POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/rbtrie/data
		${CMAKE_BINARY_DIR}/rbtrie/data
)
