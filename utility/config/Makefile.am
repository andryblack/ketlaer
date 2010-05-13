
SUBDIRS = lib doc

if BUILDEXAMPLES
SUBDIRS += examples
endif

.PHONY: dist-rpm

dist-rpm: distcheck
	rpmbuild -ta $(distdir).tar.gz

EXTRA_DIST = \
	*.sln \
	test.cfg \
	TODO \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/libconfig++9-dev.install \
	debian/libconfig++9.install \
	debian/libconfig9-dev.docs \
	debian/libconfig9-dev.install \
	debian/libconfig9.info \
	debian/libconfig9.install \
	debian/rules \
	debian/shlibs \
	debian/watch \
	libconfig.spec
