#!/bin/sh
# Run the lecture set in order. 08 takes a filename, so it is run explicitly.
#
#   ./run-all.sh        straight through, under a second
#   ./run-all.sh -p     pause for Enter between demos (use this in class)

pause=no
[ "$1" = "-p" ] && pause=yes

banner() { printf '\n\033[1m=== %s ===\033[0m\n' "$1"; }

for prog in [0-9][0-9]-*; do
	case "$prog" in
	*.c) continue ;;            # the glob sees the sources too
	08-*) continue ;;           # needs an argument; run at the end
	esac
	[ -x "$prog" ] || continue
	banner "$prog"
	"./$prog"
	[ "$pause" = yes ] && { printf '\n[Enter] '; read -r _; }
done

banner "08-mini-cat foobar.txt"
./08-mini-cat foobar.txt
printf '\n'

banner "08-mini-cat big.txt > copy.txt   (the same program is cp)"
./08-mini-cat big.txt > copy.txt
cmp big.txt copy.txt && echo "copy.txt is byte-for-byte identical to big.txt"
