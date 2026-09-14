# CSCE 313 — UNIX file I/O, in eight short programs

Companion demos for `CSCE313-8.pdf`.

Eight programs, **seven teaching beats**, 06 and 07 are one beat, run back to
back, because the contrast between them *is* the lesson.

## Quick start

```bash
make          # builds all eight
./run-all.sh  # runs them in order
make clean    # binaries and every file the demos created
```

`./run-all.sh -p` waits for Enter between demos — use that version live.

Requires `gcc` and `make` on Linux.

## The programs

| | Program | What it shows | Slide |
| --- | --- | --- | --- |
| 01 | `fd-numbers` | A descriptor is an index into a per-process table. `open()` takes the lowest free slot, `close()` hands it back | 8, 9, 10 |
| 02 | `the-table` | `/proc/self/fd` — the descriptor table, printed. 0/1/2 are ordinary rows; watch a row appear and vanish | 5, 9, 19 |
| 03 | `permissions` | `open(..., 700)` vs `open(..., 0700)`. The missing zero is a real bug, and `ls -l` shows it | 13, 14 |
| 04 | `cursor` | The "foobar" slide: two 2-byte reads give `fo` then `ob`. The kernel holds the offset, not you | 7, 16 |
| 05 | `short-counts` | `read()` returns *at most* what you asked for. The loop-until-0 idiom, which is the only correct one | 15, 17 |
| 06 | `two-opens` | Two `open()`s → two file-table entries → two independent cursors. Both read `f` | 19, 20, 22 |
| 07 | `dup-shares` | `dup()` → two descriptors, **one** file-table entry, one shared cursor. Then `dup2(fd, 1)`: redirection by hand | 21 |
| 08 | `mini-cat` | open + read loop + write loop + close. A working `cat`, and with `>` a working `cp` | — |