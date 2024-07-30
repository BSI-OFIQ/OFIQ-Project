set(FILE OFIQ-EXTERN.zip)
set(URL https://standards.iso.org/iso-iec/29794/-5/ed-1/en/OFIQ-EXTERN.zip)

message("Downloading external source code from ${URL}")
file(DOWNLOAD ${URL} ${FILE} SHOW_PROGRESS)
message("Extracting external source code")
file(ARCHIVE_EXTRACT 
    INPUT ${FILE}
    DESTINATION ../
    VERBOSE
)
file(REMOVE ${FILE})
