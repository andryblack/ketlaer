### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak

default: po

pot:
	xgettext --c++ --directory=../ --debug --files-from=POTFILES.in --output=mms.pot 

po:
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
	  echo "$$lang:"; \
	  msgfmt --statistics $$lang.po -o $$lang.mo ; \
	done

install:
	@./mkinstalldirs $(DESTDIR)$(PREFIX)/share/;
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
	  ./mkinstalldirs $(DESTDIR)$(PREFIX)/share/locale/$$lang/LC_MESSAGES/ ; \
	  $(INSTALL) -c -m 644 $$lang.mo $(DESTDIR)$(PREFIX)/share/locale/$$lang/LC_MESSAGES/mms.mo ; \
	done

clean:
	@if test -f da.mo ; then rm *.mo ; fi

distclean:
	$(MAKE) clean
	@if test -f check-size ; then rm check-size ; fi

update-po: pot
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
	  echo "Updating $$lang."; \
	  msgmerge --update $$lang.po mms.pot ; \
	done

tool:
	g++ `$(KETLAER)/bin/imlib2-config --cflags --libs` check-size.cpp -o check-size
