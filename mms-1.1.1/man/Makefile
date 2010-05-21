### The directory environment:

MMSDIR = ../

### Includes and Defines

include $(MMSDIR)/config.mak

LANGUAGES = de

install:
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
          cp $$lang mms.1 && gzip -9 mms.1; \
          if test ! -d $(DESTDIR)$(PREFIX)/share/man/$$lang/man1 ; then mkdir -p $(DESTDIR)$(PREFIX)/share/man/$$lang/man1; fi; \
          cp mms.1.gz $(DESTDIR)$(PREFIX)/share/man/$$lang/man1/; \
	  rm mms.1.gz; \
	done
	@cp en mms.1 && gzip -9 mms.1; \
          if test ! -d $(DESTDIR)$(PREFIX)/share/man/man1 ; then mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1; fi; \
        cp mms.1.gz $(DESTDIR)$(PREFIX)/share/man/man1/; \
	rm mms.1.gz; \
