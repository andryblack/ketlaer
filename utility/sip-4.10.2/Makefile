
all:
	@(cd sipgen; $(MAKE))
	@(cd siplib; $(MAKE))

install:
	@(cd sipgen; $(MAKE) install)
	@(cd siplib; $(MAKE) install)
	@test -d $(DESTDIR)/usr/local/lib/python2.6/site-packages || mkdir -p $(DESTDIR)/usr/local/lib/python2.6/site-packages
	cp -f sipconfig.py $(DESTDIR)/usr/local/lib/python2.6/site-packages/sipconfig.py
	cp -f /root/src/ketlaer/utility/sip-4.10.2/sipdistutils.py $(DESTDIR)/usr/local/lib/python2.6/site-packages/sipdistutils.py

clean:
	@(cd sipgen; $(MAKE) clean)
	@(cd siplib; $(MAKE) clean)
