CFLAGS += -Wall -Wextra
CFLAGS += -I/home/chean/Linux_gateway

SRCS += $(shell find App -type f -name "*.c")
SRCS += $(shell find Ota -type f -name "*.c")
SRCS += $(shell find Deamon -type f -name "*.c")
SRCS += $(shell find Thirdparty -type f -name "*.c")

OBJS = $(SRCS:.c=.o)

TARGET = Linux_gateway

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) $(TARGET) main.o

test_buffer:$(OBJS) Test/test_buffer.o
	-@$(CC) $(CFLAGS) -o $@ $^
	-@./$@
	-@$(RM) $@ $^

