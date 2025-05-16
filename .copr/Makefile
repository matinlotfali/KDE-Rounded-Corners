# .copr/Makefile
srpm:
	dnf install -y git

	# derive version from the latest tag, strip leading 'v'
	sed -e "s/^Version:.*/Version: $$(git describe --tags --abbrev=0 | sed 's/^v//')/" $(spec) > patched.spec

	# derive kwin package version from dnf
	sed -i -e "s/^Release:.*/Release: $$(dnf info -y kwin | awk '$$1=="Version" {print $$3}')%{?dist}/" patched.spec

	# build the SRPM into the outdir
	rpmbuild -bs \
	    --define "_sourcedir $(PWD)" \
	    --define "_srcrpmdir $(outdir)" \
	    patched.spec