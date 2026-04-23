-include Makefile.local
XCB?=xcodebuild
XCBFLAGS=-project ${KIN}.xcodeproj -scheme ${KIN} -configuration Release CONFIGURATION_BUILD_DIR=${builddir}
KIN=pumpkin
VERSION=$(shell $(XCB) ${XCBFLAGS} -showBuildSettings -json|jq '.[0].buildSettings.MARKETING_VERSION')

builddir?=build
dmgdir?=${builddir}/dmg
dmg?=${builddir}/${KIN}-${VERSION}.dmg

default:
	@echo "huh?"

${builddir}/${KIN}.app:
	$(XCB) ${XCBFLAGS}
${builddir}/dmgdir.stamp: ${builddir}/${KIN}.app
	rm -rf ${dmgdir} && mkdir ${dmgdir}
	cp -R ${builddir}/${KIN}.app ${dmgdir}/
	mkdir ${dmgdir}/.background
	cp dmg-background.png ${dmgdir}/.background/background.png
	cp dmg-DS_Store ${dmgdir}/.DS_Store
	ln -s /Applications ${dmgdir}/Applications
	touch $@
${builddir}/${KIN}-hybrid.dmg: ${builddir}/dmgdir.stamp
	hdiutil makehybrid -o $@ -hfs -hfs-openfolder ${dmgdir} -default-volume-name "PumpKIN" ${dmgdir}
${dmg}: ${builddir}/${KIN}-hybrid.dmg
	hdiutil convert -format UDZO -o $@ $<

build: ${dmg}

clean:
	rm -rf ${builddir}
