ifeq '$(findstring ;,$(PATH))' ';'
    detected_OS := Windows
else
    detected_OS := $(shell uname 2>/dev/null || echo Unknown)
    detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
    detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
    detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
endif

$(warning detected_OS is $(detected_OS))

ifeq ($(detected_OS),Windows)
    CC=x86_64-w64-mingw32-gcc.exe
endif
ifeq ($(detected_OS),Cygwin)
    CC=x86_64-w64-mingw32-gcc.exe
endif
ifeq ($(detected_OS),Darwin)        # Mac OS X
    CC=gcc
endif
ifeq ($(detected_OS),Linux)
    CC=gcc
endif
ifeq ($(detected_OS),GNU)           # Debian GNU Hurd
    CC=gcc
endif
ifeq ($(detected_OS),GNU/kFreeBSD)  # Debian kFreeBSD
    CC=gcc
endif
ifeq ($(detected_OS),FreeBSD)
    CC=gcc11
endif
ifeq ($(detected_OS),NetBSD)
    CC=gcc
endif
ifeq ($(detected_OS),DragonFly)
    CC=gcc
endif
ifeq ($(detected_OS),Haiku)
    CC=gcc
endif

CFLAGS=-O3 -I. -Wall
all: unukes usuck

unukes : unukes.o 
unukes.o : unukes.c 
	  $(CC) -c $(CFLAGS) unukes.c

usuck : usuck.o 
usuck.o : usuck.c 
	  $(CC) -c $(CFLAGS) usuck.c

clean :
	-rm -f unukes unukes.o usuck usuck.o
