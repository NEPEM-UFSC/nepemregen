cmake_minimum_required(VERSION 3.16)

# Adiciona os componentes do projeto original para serem testados
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(edge_appliance_tests)
