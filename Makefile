CFLAGS=-Wall -Wextra -ggdb -O0 -MMD -I.

libaoc:=libaoc.so
lib_dir:=/usr/local/lib
header_dir:=/usr/local/include/aoc

objects=


.DEFAULT_GOAL = $(libaoc)

objects+=die.o \
	 dlist.o \
	 incache.o \
	 lncache.o \
	 mapcache.o \
	 lut.o

$(libaoc): $(objects)
	gcc $(CFLAGS )-shared -o $@ $^ 


%.o: %.c
	gcc $(CFLAGS) -fPIC -c -o $@ $< 

#nacuna??? install libaoc to /usr/local/lib directory.
# its highly likely this directory is not included in the ldconfig's normal
# search path so for now, we will add an to entry /usr/local/lib manually 
# to /etc/ld.so.conf.d/

aoc_ldcache:=/etc/ld.so.conf.d/aoc.conf

install:
	install -m 755 $(libaoc) $(lib_dir)
	install -d -m 755 $(header_dir)
	install -m 644 aoc/*.h $(header_dir)
	echo "$(lib_dir)" | tee $(aoc_ldcache)
	ldconfig

uninstall:
	rm -rf $(header_dir)
	rm -rf $(lib_dir)/$(libaoc)
	rm -rf $(aoc_ldcache)
	ldconfig


.PHONY: clean
clean:
	rm -rf *.o
	rm -rf *.d
	rm -rf *.so

-include $(.c=.d)
