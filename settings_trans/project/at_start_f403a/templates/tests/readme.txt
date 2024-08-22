You need to send the slip_uart file to artery.
You can see structure by byte in by_byte file.
In tests 6, due to some generator bug, 
more bytes were written to the file than are actually in the structure, 
which can be seen by the size of the by_byte file. 
I'm already tired of this generator, 
maybe someday I'll figure out why this is happening