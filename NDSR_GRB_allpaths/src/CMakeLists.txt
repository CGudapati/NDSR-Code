add_library(RCSPP_graph RCSPP_graph.hpp RCSPP_graph.cpp)
add_library(NDSR_Instance NDSR_Instance.hpp NDSR_Instance.cpp)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Graph_Reader.h get_all_RCSPP.h Nodes_and_labels.h)

target_link_libraries(${CMAKE_PROJECT_NAME} RCSPP_graph NDSR_Instance)


set_target_properties(${CMAKE_PROJECT_NAME}
PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
)