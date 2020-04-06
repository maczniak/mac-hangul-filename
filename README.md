# mac-hangul-filename

make the Korean (Hangul) filename `find`-friendly for Mac command-line users

```
$ find . -name $(h '*계약서*.pdf')
$ find ~/Downloads -name `h "*리스트*"`
$ h -h
usage: h text ... - convert text into the searchable (jamo-separated) form
         -c       - (reverse) combine standard input into the normalized form
         -h       - help
```

