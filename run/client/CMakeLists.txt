INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/client)
ADD_EXECUTABLE(client client.c)
TARGET_LINK_LIBRARIES(client pthread ev msgpackc)