# P4 - Steganography
Author: Caleb Willson

Date: 11/16/22
## Information

steg privides a Linux-style command line utility used for encoding plain text into the pixel data of a png by writing each bit of the supplied text into the least significant bit of each pixel's RGBA values. While this technically causes some discoloration of the image, the effect is virtually inperceptable.

All files being used must be in the same directory as the executable if no file paths are specified. Any files created by the program will also be created in the active directory.
## Encoding

```cmd
steg -e <original image name> <modified image name> [input ASCII text file name]
```

For example:
```cmd
steg -e image.png secret.png text.txt
```
would take the contents from `text.txt`, encode it into `image.png`, and save it into `secret.png`. If `secret.png` does not exist, a new png will be created with the name specified.

If no input text file is supplied, the program will take inputs from the user until the user inputs `CTRL+Z (^Z)` on a new line.

There is a known issue where the `^Z` needs to be on its own line to end the user input. A sample output is shown below:
```cmd
steg -e image.png secret.png
Input text to encode:
This won't end the user input ^Z

This will end user input
^Z

Encoding text to secret.png
```

## Decoding

```cmd
steg -d <modified image name> [output ASCII text file name]
```

For example:
```cmd
steg -d secret.png text2.txt
```
would decode the message stored in `secret.png` and store it in the text file, `text2.txt`. If `text2.txt` does not exist, a new text file will be created.

If no output file is supplied, the decoded contents of `secret.png` will be printed to the command line.

## Complilation
Using the g++ compiler:
```cmd
g++ steg.cpp lodepng.cpp -Wall -Wextra -pedantic -ansi -O3 -o steg.exe
```

## Resources
This project makes use of LodePNG to load the binary data of a png into a vector for easy use. More information about the usage of that resource as well as more detailed compilation instructions can be found on their [github](https://github.com/lvandeve/lodepng).