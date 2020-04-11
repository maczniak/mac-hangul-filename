# mac-hangul-filename

make the Korean (Hangul) filename `find`-friendly for Mac command-line users

```
$ hfind . -name '*계약서*.pdf'
$ find . -name $(h '*계약서*.pdf')
$ find ~/Downloads -name `h "*리스트*"`
$ h -z presentations.zip
$ h -h
usage: h text ... - convert text into the searchable (jamo-separated) form
         -c       - (reverse) combine standard input into the normalized form
         -z filename - convert the zip file and backup
         -h       - help
```

## TODO

* support the tar format
* zip file listing in vi

