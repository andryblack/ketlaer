### The directory environment:

MMSDIR = ../../

### Includes and Defines

include $(MMSDIR)/config.mak

LANGUAGES = de

install:
	@languages='$(LANGUAGES)'; \
        for lang in $$languages; do \
          cp $$lang mms-movie-library.1 && gzip -9 mms-movie-library.1; \
	  if test ! -d $(DESTDIR)$(PREFIX)/share/man/$$lang/man1 ; then mkdir -p $(DESTDIR)$(PREFIX)/share/man/$$lang/man1; fi; \
	  cp mms-movie-library.1.gz $(DESTDIR)$(PREFIX)/share/man/$$lang/man1/; \
	  rm mms-movie-library.1.gz; \
	done
	@cp en mms-movie-library.1 && gzip -9 mms-movie-library.1; \
	  if test ! -d $(DESTDIR)$(PREFIX)/share/man/man1 ; then mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1; fi; \
	cp mms-movie-library.1.gz $(DESTDIR)$(PREFIX)/share/man/man1/; \
	rm mms-movie-library.1.gz;
