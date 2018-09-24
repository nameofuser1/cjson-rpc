CC = gcc
LD = gcc

INC =
INC += -I lib/
INC += -I src/

CFLAGS =
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -std=c99

LDFLAGS =
LDFLAGS += $(CFLAGS)

OBJS =
OBJS += src/main.o
OBJS += src/rpc_json_test.o
OBJS += lib/rpc_json/rpc_json.o
OBJS += lib/cJSON/cJSON.o
OBJS += lib/cJSON/cJSON_Utils.o


all: test.elf

clean:
	rm -f $(OBJS)
	rm test.elf

test.elf: $(OBJS)
	$(LD) -o test.elf $(OBJS) $(LDFLAGS)
	

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
	

.PHONY: all
