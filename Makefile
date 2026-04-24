-include Makefile.local
XCODEBUILD?=xcodebuild
XCODEBUILDFLAGS=-project ${KIN}.xcodeproj -scheme ${KIN} -configuration Release -archivePath ${archivepath} CONFIGURATION_BUILD_DIR=${builddir}
XCRUN?=xcrun
KIN=pumpkin
VERSION=$(shell $(XCODEBUILD) ${XCODEBUILDFLAGS} -showBuildSettings -json|jq '.[0].buildSettings.MARKETING_VERSION')

builddir?=build
dmgdir?=${builddir}/dmg
dmg?=${builddir}/${KIN}-${VERSION}.dmg
archivepath=${builddir}/${KIN}.xcarchive
apppath=${archivepath}/Products/Applications/${KIN}.app

default:
	@echo "huh?"

${builddir}/${KIN}.app:
	$(XCODEBUILD) ${XCODEBUILDFLAGS} archive
${builddir}/dmgdir.stamp: ${builddir}/${KIN}.app
	rm -rf ${dmgdir} && mkdir ${dmgdir}
	cp -R ${apppath} ${dmgdir}/
	mkdir ${dmgdir}/.background
	cp dmg-background.png ${dmgdir}/.background/background.png
	cp dmg-DS_Store ${dmgdir}/.DS_Store
	ln -s /Applications ${dmgdir}/Applications
	touch $@
${builddir}/${KIN}-hybrid.dmg: ${builddir}/dmgdir.stamp
	rm -f "$@"
	hdiutil makehybrid -o $@ -hfs -hfs-openfolder ${dmgdir} -default-volume-name "PumpKIN" ${dmgdir}
${dmg}: ${builddir}/${KIN}-hybrid.dmg
	rm -f "$@"
	hdiutil convert -format UDZO -o $@ $<
	$(XCRUN) notarytool submit $@ --keychain-profile ${AC_KEYCHAIN_PROFILE} --wait
	$(XCRUN) stapler staple $@

build: ${dmg}

clean:
	rm -rf ${builddir}
