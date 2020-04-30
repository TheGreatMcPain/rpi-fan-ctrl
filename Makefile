ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

ifneq ($(filter default undefined,$(origin CC)),)
    CC = gcc
endif

export CC

CFLAGS := ${CFLAGS} -Wall -Werror

TARGET = rpi-fan-ctrl

all: $(TARGET)

$(TARGET): rpi-fan-ctrl.c
	$(CC) $(CFLAGS) -lpigpio -lpthread -o $(TARGET) $(TARGET).c

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/sbin/
	install $(TARGET) $(DESTDIR)$(PREFIX)/sbin/

clean:
	$(RM) $(TARGET)
