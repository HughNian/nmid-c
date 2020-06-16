INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/worker)
ADD_EXECUTABLE(worker worker.c)
TARGET_LINK_LIBRARIES(worker pthread ev msgpackc)