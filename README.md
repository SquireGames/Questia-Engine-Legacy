# Questia Engine

Questia Engine is a game engine that provides a GUI system, tile engine, and resource management. It uses the SFML 2.4.2 Libraries to handle window management, rendering, networking and audio.
It uses Crypto++ Library 5.6.5 to encrypt sensitive data. It uses AngelScript 2.31.2 for all scripting. It uses UnitTest++ 2.0 for testing, but is only linked to the test executable.

Building:
----------------
Questia Engine currently supports Windows and Linux OS. A compilation guide is found [here](https://github.com/SquireGames/Questia-Engine/wiki/Compling-guide).


Testing:
----------------
To run the tests, first add all the necessary libraries into the directory _tests_ (other than the engine, that gets copied into the directory automatically), and then run `make test`. 


License:
----------------
This project is licensed under the terms of the GNU GPLv3 license, see LICENSE.txt.
