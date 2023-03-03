# Enigma

A functioning Enigma machine simulation I created one night when my homework cleared up faster than I had imagined. I figured it'd be a good test of my ability to code in C, as it has been a while.

The code is a little rough at the moment (I roughed out the main functionality between the hours of midnight and 4am), so if someone is actually looking at this, go easy on me...

Currently a work in progress, but comes fairly fully featured, with the option of running from the command-line or in an interactive mode.

As for realism, I have created rotors I->V, as well as a full plugboard and all the standard ring settings. I have also tested samples run on validated machines as proof that my model operates properly, receiving identical outputs.

Future work (potentially):

- Breaking some of the structs and functions into separate files
- Adding more optional ring settings that I wasn't originally aware of
- Some sort of ascii visualization?
- Adding the remaining rotors from the later models (like the navy version)
- Making a Bombe to try and decrypt the messages with?
