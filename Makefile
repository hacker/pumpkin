VERSION=0.0
KIN=pumpkin
PACKAGE=${KIN}-${VERSION}
TARNAME=${PACKAGE}-osx
TARS=$(addprefix ${TARNAME}.tar.,gz bz2) ${TARNAME}.tar

dist: ${TARS}
clean:
	rm -f ${TARS}

${TARNAME}.tar.gz: ${TARNAME}.tar
	gzip -v9 <"$<" >"$@"
${TARNAME}.tar.bz2: ${TARNAME}.tar
	bzip2 -v9 <"$<" >"$@"
${TARNAME}.tar:
	git archive --format tar -o "$@" --prefix="${PACKAGE}/" HEAD

.INTERMEDIATE: ${TARNAME}.tar
