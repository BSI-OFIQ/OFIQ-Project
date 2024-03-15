function(download_file url file)

message(STATUS "Downloading '${file}'")
file(DOWNLOAD ${url}  ${file})

message(STATUS "Extracting '${file}' to '${PROJECT_SOURCE_DIR}/extern'")
file(ARCHIVE_EXTRACT 
        INPUT ${file}
        DESTINATION ${PROJECT_SOURCE_DIR}/extern
        VERBOSE
    )
file(REMOVE ${file})
endfunction(download_file)