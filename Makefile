
ifeq ($(wildcard config.mk),config.mk)
include config.mk
ifdef WITH_DOCS
first: butt docs
else
first: butt
endif
else
first:
	@echo "run './configure' first"
endif

clean:
	$(MAKE) -C src clean

dist:
#Make the binary package
	$(MAKE) -C src distclean
	./configure --static
	$(MAKE) -C src
	mkdir $(DIST_VER)-linux-bin
	cp -R src/butt player_plugins/ ChangeLog README LICENSE KNOWN_BUGS \
	install.sh uninstall.sh $(DIST_VER)-linux-bin/

	tar -cjf $(DIST_VER)-linux-bin.tar.bz2 $(DIST_VER)-linux-bin
	rm -rf $(DIST_VER)-linux-bin

#Make the source package
	$(MAKE) -C src distclean
	mkdir $(DIST_VER)
	cp -R src/ player_plugins/ configure* Makefile ChangeLog KNOWN_BUGS \
	INSTALL README LICENSE $(DIST_VER)/
	tar -cjf $(DIST_VER)-src.tar.bz2 $(DIST_VER)
	rm -rf $(DIST_VER)

distclean:
	$(MAKE) -C src distclean
	rm -f config.mk config.log tmp.c

install: butt
	cp -fv src/butt $(DESTDIR)$(prefix)/bin/butt

uninstall:
	rm -fv $(DESTDIR)$(prefix)/bin/butt

butt:
	$(MAKE) -C src
