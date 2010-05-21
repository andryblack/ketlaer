### The directory environment:

MMSDIR = ../../../../

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
	@../../../../po/mkinstalldirs $(DESTDIR)$(PREFIX)/share/;
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
	  ../../../../po/mkinstalldirs $(DESTDIR)$(PREFIX)/share/locale/$$lang/LC_MESSAGES/ ; \
	  $(INSTALL) -c -m 644 $$lang.mo $(DESTDIR)$(PREFIX)/share/locale/$$lang/LC_MESSAGES/mms-audio.mo ; \
	done

clean:
	@if test -f da.mo ; then rm *.mo ; fi

distclean:
	$(MAKE) clean

update-po: pot
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
	  echo "Updating $$lang."; \
	  msgmerge --update $$lang.po mms.pot ; \
	done
