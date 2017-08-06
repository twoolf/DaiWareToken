DIR=$(DESTDIR)/opt/iot
LIBDIR=$(DESTDIR)/usr/lib
CFGDIR=$(DESTDIR)/etc/iotsample-raspberrypi

COMPILELIBDIR=./lib
LIBNAME=paho-mqtt3as

CFLAGS ?= -Wall -W -O2

all: iot

.PHONY: all install clean distclean

iot: iotmain.c cpustat.c mac.c mqttPublisher.c jsonator.c iot.h jsonReader.c
	export LIBRARY_PATH=$(COMPILELIBDIR):$(LIBRARY_PATH)
	$(CC) $(CFLAGS) iotmain.c cpustat.c mac.c mqttPublisher.c jsonator.c cJSON.c jsonReader.c -o $@ -l$(LIBNAME) -lpthread -lssl -lm -L $(COMPILELIBDIR) -I .
	strip $@

install: iot
	mkdir -p $(LIBDIR)
	mkdir -p $(DIR)
	mkdir -p $(CFGDIR)
	install iot $(DIR)/iot
	cp $(COMPILELIBDIR)/libpaho-mqtt3a.so.1.0 $(LIBDIR)/libpaho-mqtt3a.so.1.0
	cp $(COMPILELIBDIR)/libpaho-mqtt3as.so.1.0 $(LIBDIR)/libpaho-mqtt3as.so.1.0
	ln -s libpaho-mqtt3a.so.1.0 $(LIBDIR)/libpaho-mqtt3a.so.1
	ln -s libpaho-mqtt3as.so.1.0 $(LIBDIR)/libpaho-mqtt3as.so.1
	ln -s libpaho-mqtt3a.so.1 $(LIBDIR)/libpaho-mqtt3a.so
	ln -s libpaho-mqtt3as.so.1 $(LIBDIR)/libpaho-mqtt3as.so
	install iotGetDeviceID.sh $(DIR)/iotGetDeviceID.sh
	cp README.md $(DIR)/README
	cp IoTFoundation.pem $(DIR)/IoTFoundation.pem

clean:
	rm -f iot
