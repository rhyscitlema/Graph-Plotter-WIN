
Software name:
    Rhyscitlema Graph Plotter 3D

Software description:
    Draw any graph in a 3D virtual space.


To compile on Windows with MinGW:

1) First get the folders:
    * ..\algorithms
    * ..\lib_std
    * ..\librfet
    * ..\librodt
    * ..\read_write_image_file

2) Install MinGW. Instructions are found at:
   http://www.mingw.org/wiki/Getting_Started
   By default it will install at C:\MinGW\

3) Add the folder C:\MinGW\bin to
   the PATH environment variable.
   Go onto the command line interface
   and execute: SET PATH=C:\MinGW\bin

4) Go to ..\read_write_image_file and compile
   as per the instructions provided there for
   compiling for the Microsoft Windows platform.

5) While in the command line interface terminal,
   change directory to here, then finally execute:
    mingw32-make.exe
or  mingw32-make.exe CFLAGS="-DNDEBUG -O2" LDLIBS="-s"


To run, execute:
    Graph_Plotter_3D.exe

To delete all created files, execute:
    mingw32-make.exe clean RM=del


Provided by Rhyscitlema
@ http://rhyscitlema.com

USE AT YOUR OWN RISK!
