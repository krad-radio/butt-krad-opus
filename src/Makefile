##########################
$(info check for ../config.mk ...)
ifeq ($(wildcard ../config.mk),../config.mk)
$(info ok.)
include ../config.mk
endif

##########################
define funcAddModule
	ifeq ($(WITH_$(1)),1)
		SRC += $$(SRC_$(1))
		CXXFLAGS += -I$(1) $$(CFLAGS_$(1)) $$(CXXFLAGS_$(1)) -DHAVE_$(1)
		LDFLAGS += $$(LDFLAGS_$(1))
		LIBS += $$(LIBS_$(1))
	endif
endef
##########################

DEBUG= 
MAKEDEP ?= g++ -MM -I. -I/usr/local/include -IFLTK
CXX ?= g++
CXXFLAGS += $(DEBUG) -I. -I/usr/local/include -Wall -O2 -DVERSION=$(VERSION) ``
LDFLAGS += -L/usr/local/lib `` #-lduma #-ldiehard_r 
LIBS ?= 

##########################

SRC =	butt.cpp port_audio.cpp \
	shoutcast.cpp icecast.cpp parseconfig.cpp \
	cfg.cpp timer.cpp strfuncs.cpp \
	sockfuncs.cpp util.cpp wav_header.cpp \
	ringbuffer.cpp
	
######################

######################

MODULES = FLTK QT PORTAUDIO LIBVORBIS LIBLAME
$(foreach module,$(MODULES),$(eval $(call funcAddModule,$(module))))

#######################


#######################
SOURCES = $(sort $(SRC))
OBJECTS = $(SOURCES:.cpp=.o)
DEPS    = $(foreach d,$(SOURCES), .$(d:.cpp=.d))
TARGET  = butt

#######################


#######################

$(TARGET) : $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

butt.o : ../config.mk

clean:
	@rm -f $(OBJECTS) *~ FLTK/*~ QT/*~

distclean:
	rm -f $(TARGET) $(OBJECTS) *~ FLTK/*~ QT/*~ .*.d

vimclean:
	rm -f .*sw*      #cleanup vim swapfiles
	rm -f FLTK/.*sw*


deps: $(DEPS)

#.%.d : %.cpp
#	$(MAKEDEP) $(CFLAGS) -o $@ $<

#######################

ifneq ($(MAKECMDGOALS),deps)
ifneq ($(MAKECMDGOALS),first)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean)
     -include $(DEPS)
endif
endif
endif
endif

#######################
#######################

