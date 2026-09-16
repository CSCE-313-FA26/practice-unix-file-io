# CSCE 313 — UNIX file I/O, in eight short programs

Companion demos for `CSCE313-8.pdf`.

## Quick start

```bash
make          # builds all eight
./run-all.sh  # runs them in order
make clean    # binaries and every file the demos created
```

`./run-all.sh -p` waits for Enter between demos.

Requires `gcc` and `make` on Linux.

If `./run-all.sh`
says `Permission denied`, the script lost its execute bit in transit:
`chmod +x run-all.sh`.

## The programs

| | Program | What it shows | Slide |
| --- | --- | --- | --- |
| 01 | `fd-numbers` | A descriptor is an index into a per-process table. `open()` takes the lowest free slot, `close()` hands it back | 8, 9, 10 |
| 02 | `the-table` | `/proc/self/fd` — the descriptor table, printed. 0/1/2 are ordinary rows; watch a row appear and vanish | 9, 10 |
| 03 | `permissions` | `open(..., 700)` vs `open(..., 0700)`. The missing zero is a real bug, and `ls -l` shows it | 11–14 |
| 04 | `cursor` | The "foobar" slide: two 2-byte reads give `fo` then `ob`. The kernel holds the offset, not you | 15, 16 |
| 05 | `write-twice` | The "abab" slide: the same two bytes written twice give `abab`. `write()` moves that same cursor. Plus the `sizeof` trap the slide raises | 17, 18 |
| 06 | `two-opens` | Two `open()`s → two file-table entries → two independent cursors. Both read `f` | 19–22 |
| 07 | `dup-shares` | `dup()` → two descriptors, **one** file-table entry, one shared cursor. Then `dup2(fd, 1)`: redirection by hand | past 22 |