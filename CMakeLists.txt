#c与c++混合编译(c也用g++编译)
SET(CMAKE_C_COMPILER  g++ )

#project name

PROJECT(MusicRobot)

#head file path

INCLUDE_DIRECTORIES(
include
)

#source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
#FILE(GLOB_RECURSE SRC_LIST "*.cpp")

#set extern libraries

SET(LIBRARIES1 sndfile)
SET(LIBRARIES2 fftw3f)
SET(LIBRARIES3 asound)
SET(LIBRARIES4 usb-1.0)


#add executable file

ADD_EXECUTABLE(../bin/MusicRobot ${DIR_SRCS})


#add link library

TARGET_LINK_LIBRARIES(../bin/MusicRobot ${LIBRARIES1})
TARGET_LINK_LIBRARIES(../bin/MusicRobot ${LIBRARIES2})
TARGET_LINK_LIBRARIES(../bin/MusicRobot ${LIBRARIES3})
TARGET_LINK_LIBRARIES(../bin/MusicRobot ${LIBRARIES4})

