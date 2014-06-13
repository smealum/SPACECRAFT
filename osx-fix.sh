#!/bin/bash

# fix quand il ne trouve pas certaines dylib. Cela peut arriver...
# $1 = executable

while [[ ! "$1" = "" ]]; do
	EXE="$1"

	echo "Checking for $EXE..."
	BAD_LIBS=$(otool -L "$EXE" | cut -d " " -f 1 | grep "@executable")

	for i in $BAD_LIBS; do
		echo "Fixing $i for $EXE..."
		NEW_LIB=`locate $(basename $i) | tail -n1`
		echo "Found $NEW_LIB for $EXE, patching..."
		if [[ "$?" = 0 ]]; then
			install_name_tool -change "$i" "$NEW_LIB" "$EXE" || echo "ERROR: cannot change $EXE"
		else
			echo "ERROR: couldn't locate '$i' for $EXE"
		fi
	done
	shift;
done
