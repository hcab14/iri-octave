# iri-octave
[International Reference Ionosphere](http://irimodel.org) octave binding

## prerequisites:
* wget
* [octave](https://www.gnu.org/software/octave/)

## build
* the original IRI2016 code is not touched (and not part of this git repository), instead
* `make` downloads IRI code and data files from http://irimodel.org and compiles iri.oct
* when not using `bash` (e.g. `zsh`) build with `make SHELL=bash`

## example octave script
* see [iri_test.m](https://github.com/hcab14/iri-octave/blob/master/m/iri_test.m)
