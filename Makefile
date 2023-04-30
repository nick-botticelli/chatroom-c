# Many thanks to Job Vranish - see https://spin.atomicobject.com/2016/08/26/makefile-c-projects/
TARGET_EXEC := chat_node
BUILD_DIR := ./build
SRC_DIRS  := ./src
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS  := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS := $(INC_FLAGS) -MMD -MP -Wall -Wno-unused-command-line-argument -Wno-undefined-inline
LDFLAGS  := -pthread -lpthread

RELEASE_FLAGS := -Ofast -DNDEBUG -fvisibility=hidden -fstack-protector-strong \
-fomit-frame-pointer -fPIE -fstack-clash-protection -fsanitize=bounds \
-fsanitize-undefined-trap-on-error -D_FORTIFY_SOURCE=3 -flto
# RELEASE_LDFLAGS := -Wl,-z,relro,-z,now,-z,noexecstack,-z,separate-code

DEBUG_FLAGS := -O0 -g3 -fno-omit-frame-pointer -fno-common -fno-optimize-sibling-calls
ASAN_FLAGS := $(DEBUG_FLAGS) -fsanitize=address,undefined
MSAN_FLAGS := $(DEBUG_FLAGS) -fsanitize=memory -fsanitize-memory-track-origins=2
TSAN_FLAGS := $(DEBUG_FLAGS) -fsanitize=thread

.PHONY: all debug asan msan tsan clean

all: CPPFLAGS += $(RELEASE_FLAGS)
# all: LDFLAGS += $(RELEASE_LDFLAGS)
all: $(BUILD_DIR)/$(TARGET_EXEC)
	strip $(BUILD_DIR)/$(TARGET_EXEC)

debug: CPPFLAGS += $(DEBUG_FLAGS)
debug: $(BUILD_DIR)/$(TARGET_EXEC)

asan: CPPFLAGS += $(ASAN_FLAGS)
asan: $(BUILD_DIR)/$(TARGET_EXEC)

msan: CPPFLAGS += $(MSAN_FLAGS)
msan: $(BUILD_DIR)/$(TARGET_EXEC)

tsan: CPPFLAGS += $(TSAN_FLAGS)
tsan: $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
MKDIR_P ?= mkdir -p
