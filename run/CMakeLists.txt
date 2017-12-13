
Set(Exe_Names
  o2sim
  runTPC
  O2SimDeviceRunner
  O2PrimaryServerDeviceRunner
  O2HitMergerRunner
  O2ITSDigitizerDeviceRunner
  O2RunSystem
)

Set(Exe_Source
  o2sim.cxx
  runTPC.cxx
  O2SimDeviceRunner.cxx
  O2PrimaryServerDeviceRunner.cxx
  O2HitMergerRunner.cxx
  O2ITSDigitizerDeviceRunner.cxx
  O2RunSystem.cxx
)

set(BUCKET_NAME "run_bucket")
set(LIBRARY_NAME "")

list(LENGTH Exe_Names _length)
math(EXPR _length ${_length}-1)

foreach (_file RANGE 0 ${_length}) # loop over a range because we traverse 2 lists and not 1
  list(GET Exe_Names ${_file} _name)
  list(GET Exe_Source ${_file} _src)
  O2_GENERATE_EXECUTABLE(
      EXE_NAME ${_name}
      SOURCES ${_src}
      MODULE_LIBRARY_NAME ${LIBRARY_NAME}
      BUCKET_NAME ${BUCKET_NAME}
  )
endforeach (_file RANGE 0 ${_length})

#O2_GENERATE_EXECUTABLE(
  #EXE_NAME ${Exe_Names}
  #SOURCES ${Exe_Source}
  #MODULE_LIBRARY_NAME ${LIBRARY_NAME}
  #BUCKET_NAME ${BUCKET_NAME}
#)

