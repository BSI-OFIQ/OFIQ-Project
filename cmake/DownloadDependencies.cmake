function(download_file url file)
file(DOWNLOAD ${url}  ${file})

file(ARCHIVE_EXTRACT 
        INPUT ${file}
        DESTINATION ${PROJECT_SOURCE_DIR}/extern
        VERBOSE
    )
file(REMOVE ${file})
endfunction(download_file)