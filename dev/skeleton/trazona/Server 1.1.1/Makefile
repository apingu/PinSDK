include $(ZONA_HOME)/tools/UnixBuild/Makefile.def

###########################
# CCOPTION
###########################

CCOPTIONS	= $(CSHAREDFLAGS)


###########################
# INCLUDE FILES
###########################

COMMON_INCLUDES	= -I$(ZONA_HOME)/csrc/samples/Common \
		  -I$(ZONA_HOME)/csrc/samples/Common/ZonaBattle

JNI_INCLUDES 	= -I$(ZONA_HOME)/csrc/jni/zonaserverplugin

INCLUDES 	= $(JAVA_INCLUDES) $(COMMON_INCLUDES) $(JNI_INCLUDES)


###########################
# LIBRARIES
###########################

JNIPLUGIN_LIB 	= $(ZONA_HOME)/csrc/jni/zonaserverplugin/libZonaServerPlugin.a
# LDEXTRACTEDLIBS = $(PLUGIN_LIBS)

SYSLIBS 	= -lm -lstdc++


###########################
# FILES
###########################

EXTRA_OBJS = NativeStreamProcessor.o \
	zona_server_plugin_GameStateValidator.o \
	ZonaGSPublish.o \
	ZonaTimeSync.o

SRCS = Entity.cpp \
	Sector.cpp \
	VehicleFactory.cpp \
	EntityFactory.cpp \
	SectorMgr.cpp \
	Weapon.cpp \
	EntityMgr.cpp \
	Vehicle.cpp \
	ZonaGSValidate.cpp \
	$(CMN_SRCS)

OBJS = Entity.o \
	Sector.o \
	VehicleFactory.o \
	EntityFactory.o \
	SectorMgr.o \
	Weapon.o \
	EntityMgr.o \
	Vehicle.o \
	ZonaGSValidate.o \
	$(CMN_OBJS) \
	$(EXTRA_OBJS)

CMN_SRCS = 3d.cpp \
	vehicleTables.cpp \
	collision.cpp \
	ConfigReader.cpp \
	GssRegionData.cpp \
	loadASE.cpp \
	ZGlobal.cpp \
	ZonaXml.cpp

CMN_OBJS = 3d.o \
	vehicleTables.o \
	collision.o \
	ConfigReader.o \
	GssRegionData.o \
	loadASE.o \
	ZGlobal.o \
	ZonaXml.o

TARGET = libZonaBattle_ServerPlugIn

all: includes extract $(TARGET).so

$(TARGET).so: $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJS)
	$(RANLIB)  $(TARGET).a 
	$(CC) $(LDSHAREDFLAGS) -o $@ $(LDEXTRACTFLAGS) $(TARGET).a $(LDDEFALTFLAGS) $(LIBS)

clean::
	$(RM) $(OBJS) $(TARGET).so $(TARGET).a

install: $(TARGET).so
	@if [ -d $(TARGET_DIR)/lib ]; then set +x; \
	else (set -x;  $(MK) $(TARGET_DIR)/lib); fi
	$(INSTALL) $(TARGET).so $(TARGET_DIR)/lib/$(TARGET).so.$(ZBATTLE_VERSION)
	(cd $(TARGET_DIR)/lib && $(RM) $(TARGET).so && $(LN) $(TARGET).so.$(ZBATTLE_VERSION) $(TARGET).so)


###########################
# EXTRACT OBJS
###########################

$(EXTRA_OBJS):
	$(AR) $(EXTRAFLAGS) $(JNIPLUGIN_LIB)

extract: $(EXTRA_OBJS)


###########################
# IMPORTED FILES
###########################

vehicleTables.cpp:: $(ZONA_HOME)/csrc/samples/Common/ZonaBattle/vehicleTables.cpp
	$(RM) $@
	$(LN) $? $@

includes:: vehicleTables.cpp

clean::
	$(RM) vehicleTables.cpp

3d.cpp:: $(ZONA_HOME)/csrc/samples/Common/3d.cpp
	$(RM) $@
	$(LN) $? $@

includes:: 3d.cpp

clean::
	$(RM) 3d.cpp

collision.cpp:: $(ZONA_HOME)/csrc/samples/Common/collision.cpp
	$(RM) $@
	$(LN) $? $@

includes:: collision.cpp

clean::
	$(RM) collision.cpp

ConfigReader.cpp:: $(ZONA_HOME)/csrc/samples/Common/ConfigReader.cpp
	$(RM) $@
	$(LN) $? $@

includes:: ConfigReader.cpp

clean::
	$(RM) ConfigReader.cpp

GssRegionData.cpp:: $(ZONA_HOME)/csrc/samples/Common/GssRegionData.cpp
	$(RM) $@
	$(LN) $? $@

includes:: GssRegionData.cpp

clean::
	$(RM) GssRegionData.cpp

loadASE.cpp:: $(ZONA_HOME)/csrc/samples/Common/loadASE.cpp
	$(RM) $@
	$(LN) $? $@

includes:: loadASE.cpp

clean::
	$(RM) loadASE.cpp

ZGlobal.cpp:: $(ZONA_HOME)/csrc/samples/Common/ZGlobal.cpp
	$(RM) $@
	$(LN) $? $@

includes:: ZGlobal.cpp

clean::
	$(RM) ZGlobal.cpp

ZonaXml.cpp:: $(ZONA_HOME)/csrc/samples/Common/ZonaXml.cpp
	$(RM) $@
	$(LN) $? $@

includes:: ZonaXml.cpp

clean::
	$(RM) ZonaXml.cpp
