program_NAME        := epoll_usertap
program_INSTALL_DIR :=
program_SRC_DIR     := ./
program_OBJ_DIR     := objs

###############################################################################
program_INCLUDE_DIRS :=
program_LIBRARY_DIRS :=
program_LIBRARIES    :=
program_CFLAGS       := -pthread -g

###############################################################################
program_TARGET := ./$(program_INSTALL_DIR)/$(program_NAME)
program_SRCS   += $(foreach srcdir, $(program_SRC_DIR), $(wildcard $(srcdir)/*.c))
program_OBJS   := $(addprefix $(program_OBJ_DIR)/, $(program_SRCS:.c=.o))
program_DEPS   := $(program_OBJS:.o=.d)

###############################################################################
CFLAGS += $(foreach includedir, $(program_INCLUDE_DIRS), -I$(includedir))
CFLAGS += $(program_CFLAGS)

###############################################################################
LDFLAGS += $(foreach librarydir, $(program_LIBRARY_DIRS), -L$(librarydir))
LDFLAGS += $(foreach library, $(program_LIBRARIES), -l$(library))

###############################################################################
###############################################################################
.PHONY: all clean distclean

all: $(program_TARGET)

$(program_TARGET): $(program_OBJS)
	@test -d $(program_INSTALL_DIR) || mkdir -p $(program_INSTALL_DIR)
	@$(CC) $(CFLAGS) $(program_OBJS) -o $(program_TARGET) $(LDFLAGS)
	@echo "\033[1;33m[MAKE] Project is built successfully\033[0m"

$(program_OBJS): $(program_OBJ_DIR)/%.o : %.c
	@test -d $(program_OBJ_DIR) || mkdir -p $(program_OBJ_DIR)
	@$(CC) -c -MMD -o $@ $<

clean:
	@$(RM) -f $(program_TARGET)
	@$(RM) -rf $(program_OBJ_DIR)
	@echo "\033[1;32m[MAKE] Project is clean\033[0m"

distclean: clean

###############################################################################
-include $(program_DEPS)
