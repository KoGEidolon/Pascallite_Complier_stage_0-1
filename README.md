# Pascallite Complier stage 0 - 1

- This is a team project for class CS 4301 - Angelo State University - A Pascallite Complier including stage 0 - 1 - this is meant for educational purpose only.

- Why use Visual Studio here? Helps a lot with the debugging (F5) process, and make use of the intellisense in the IDE.

- In case the Visual Studio did not configure itself:
    - Go to Project Property -> Linker -> System -> change the SubSystem setting into Console(/SUBSYSTEM:CONSOLE) (since we are using main(int argc, char** argv)).
    - Go to Project Property -> Configuration Properties -> VC++ Directories ->  Include Directories section: add a new include directories target into your Project directory (or where the .h header file is).
    - Go to Project Property -> Configuration Properties -> C/C++ Preprocessor Definitions -> add "_CRT_SECURE_NO_WARNINGS" to avoid unsafe warning -- ctime function / or you can use ctime_s instead.

- Add the .dat file to the project folder, change the execute command in Project Property -> Configuration Properties -> Debugging -> change the command arguments into .dat .lst .asm. A example of 001.dat 001.lst 001.asm has been added. You will have to add the test database yourself.

- Made by KE_Darkness -- feel free to shoot me any question through huan161299@gmail.com
