add_executable(st "suffix-tree.cpp" "suffix-tree.h" "red_black_tree.h" "suffix_tree.h" "re-suffix.h" "suffix-arr.h"  )

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET st PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(st PRIVATE uni-algo::uni-algo)

add_custom_command(TARGET st POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/st/data
		${CMAKE_BINARY_DIR}/st/data
)
