CC := gcc
CFLAGS := -Wall -pthread

HDRS := mongoose/mongoose.h

SRCS := implant.c mongoose/mongoose.c
OBJS := $(SRCS:.c=.o)
EXEC := implant.build

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

#clean:
#	rm -f $(OBJS)
#	# rm -f $(EXEC) $(OBJS)

#.PHONY: all clean
