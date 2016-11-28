
A not-so-great initial implementation of a drawing library which renders into 
a framebuffer with one pixel being mapped to a bit. 

Current outstanding work:

 * No text rendering
 * No circle implementation
 * Diagonal lines are mostly broken. 45 deg seems to work OK.
 * No line width.


Text rendering:

http://www.proggyfonts.net/download/

http://askubuntu.com/questions/205969/how-do-i-port-a-fon-font-to-a-linux-font

 * write a parser for fon fonts??? need to parse into a structure which can be
 readily written into frame_buffer
