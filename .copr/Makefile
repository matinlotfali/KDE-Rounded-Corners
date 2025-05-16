# .copr/Makefile
srpm:
	dnf install -y git

	# derive version from the latest tag, count revisions, strip leading 'v', patch spec
	LAST_RELEASE_TAG=$$(git describe --tags --abbrev=0); \
    REV_COUNT=$$(git rev-list --count $${LAST_RELEASE_TAG}..HEAD); \
    PKG_VERSION=$$(echo $${LAST_RELEASE_TAG} | sed 's/^v//'); \
	sed -e "s/^Version:.*/Version: $${PKG_VERSION}.$${REV_COUNT}/" $(spec) > patched.spec

	# derive kwin package version from dnf
	sed -i -e "s/^Release:.*/Release: $$(dnf info -y kwin | awk '$$1=="Version" {print $$3}')%{?dist}/" patched.spec

	# build the SRPM into the outdir
	rpmbuild -bs \
	    --define "_sourcedir $(PWD)" \
	    --define "_srcrpmdir $(outdir)" \
	    patched.spec