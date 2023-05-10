# SIMPLE-READELF

A simple readelf program that does basic things to analyse and display information about ELF files.

## Get Started

Run `make` in order to compile the program.
```shell
$ make
```
Run `make debug` in order to compile the program with debug information and fsanitize active.
```shell
$ make debug
```

## How it works ?
The name of the output program is `simple-readelf`.
- Display header
```shell
$ ./simple-readelf -h <elf_file>
```
- Display program headers
```shell
$ ./simple-readelf -P <elf_file>
```
- Display section headers
```shell
$ ./simple-readelf -S <elf_file>
```
- Display symbol table
```shell
$ ./simple-readelf -s <elf_file>
```
- Display dynamic sections
```shell
$ ./simple-readelf -d <elf_file>
```
- Display all
```shell
$ ./simple-readelf -a <elf_file>
```

