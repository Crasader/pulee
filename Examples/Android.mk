FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Constants/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/CustomGUI/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Helpers/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Scenes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/Singletons/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/GameModels/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/json/*.c)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)

LOCAL_SRC_FILES := hellocpp/main.cpp
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)
