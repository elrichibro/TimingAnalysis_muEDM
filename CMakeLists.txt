cmake_minimum_required(VERSION 3.14)

project(TimingAnalysis_muEDM CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT REQUIRED COMPONENTS ROOTDataFrame Core Hist Tree RIO MathCore RooFit RooFitCore Minuit Unfold)
find_package(nlohmann_json REQUIRED)

include_directories(include)

set(COMMON_SRCS
    src/Config.cpp
    src/Filter.cpp
    src/Visualizer.cpp
)

set(COMMON_LIBS
    ROOT::ROOTDataFrame
    nlohmann_json::nlohmann_json
    ROOT::Core 
    ROOT::Hist 
    ROOT::Tree 
    ROOT::RIO 
    ROOT::MathCore 
    ROOT::RooFit 
    ROOT::RooFitCore
    ROOT::Minuit
    ROOT::Unfold
)

include(FetchContent)

add_executable(analyze src/main.cpp ${COMMON_SRCS})
target_link_libraries(analyze PRIVATE ${COMMON_LIBS})

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    
    doxygen_add_docs(
        docs
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        COMMENT "Generating Doxygen doc..."
    )
else()
    message(WARNING "Doxygen not finded.")
endif()