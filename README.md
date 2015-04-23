# block

C program to output text into block letters

Configuration: See #defines near top of block.c to change the pixels. font.h can be modified to add new characters as well, as it currently only supports a limited subset of punctuation and symbols, and will convert all characters to upper case.

Installation: gcc block.c -o block

Usage: 
```
 $ block "sup doood"
 $ block sup doood
 $ block this is a very very long string that goes past the end of your console but it successfully wraps around because I spent way too much time implementing that
 $ sudo apt-get install pastebinit firefox 
 $ firefox `block hello world | pastebinit`
 ```
