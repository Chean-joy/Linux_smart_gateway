CFLAGS += -Wall -Wextra
CFLAGS += -I/home/chean/Linux_gateway
# 2. 将库链接选项移出 CFLAGS（或者定义新的变量）
LDFLAGS += -L/usr/lib/x86_64-linux-gnu
LDLIBS += -lpaho-mqtt3c

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

test_mqtt:$(OBJS) Test/test_mqtt.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	-@./$@
	-@$(RM) $@ $^	

test_message:$(OBJS) Test/test_message.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	-@./$@
	-@$(RM) $@ $^	


test_task:$(OBJS) Test/test_task.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	-@./$@
	-@$(RM) $@ $^

