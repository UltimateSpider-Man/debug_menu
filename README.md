# Ultimate Spider-Man Debug menu for PC 

**Recreation** of the PS2 and XBOX Preview version's debug menu for the PC version

## Download

Get the console developer from [here](https://github.com/UltimateSpider-Man/debug_menu/releases/tag/v1)

Get the debug menu from [here](https://github.com/UltimateSpider-Man/debug_menu/releases)

Get the debug menu asi version from [here](https://github.com/UltimateSpider-Man/debug_menu/releases/tag/v1.2)


#Note console developer is not present in this project 2 projects separated openusm and debug menu


## Requirements DLL Version

* Visual C++ 2015
* CFF Explorer

## Requirements ASI Version

* Visual C++ 2015


## Installation


## DLL version for debug menu

* Download console.dll and debug_menu.dll then use CFF Exporer
* Then open USM.exe with CFF Exporer go to import adder option
* Load a dll for inject in this case open debug_menu.dll
* Click Import by Ordinal or by Name and Rebuild Database
* Then Save the USM.exe
* enjoy

## ASI version for debug menu

* Download console.dll and debug_menu.asi
* Put this 2 files on game directory
* Enjoy

 ## Switching Debug Menu to Console Developer 

 * Open cmd.exe
 * go to game folder on cmd
 * and digit this command USM --console

## Usage

* **INSERT** - opens and closes the menu
* **UP/DOWN Arrows** - moves the current select option up and down
* **ENTER** - executes the selected option
* **LEFT/RIGHT Arrows** - changes the value of the current **boolean** option
* **ESCAPE** - go to previous menu

## Gamepad Controls

* **R3** - opens and closes the menu
* **UP/DOWN Sticks** - moves the current select option up and down
* **CROSS** - executes the selected option
* **LEFT/RIGHT d-pads** - changes the value of the current **boolean** option
* **CIRCLE** - go to previous menu

## Current implemented options

### Dvars

Allows to edit game camera distances


### Warp

Allows to teleport to any in-game region

**NOTE**: Points of Interest(POI) are only used in missions and the option only works when the direction arrow is visible 

### District variants

Allows to switch between variants of given regions


### Entity variants

Allow to view only hash codes of entity variants

### Replay

This menu useless because if you press Start Option nothing happens

### Missions

Allows replay any story missions and combat tours


### Level Select

Allows to load specific levels. The default level is 'city'

this options inside the level select menu not works


### Hero Select

Allows to switch between *all* available characters

**NOTE**: `peter_parker` and `peter_hooded` are versions of peter without powers, used in the first missions of the game

### Game

Allows to modify in-game flags that controls certain behaviours

### Devopt

Allows to modify Devopt that controls certain behaviours

* Live in Glass House: disables the `Area unavailable` messages

### Devopt

Allows to modify Devopt that controls certain behaviours

* Live in Glass House: disables the `Area unavailable` messages


### Script

Access to in-game scripts that allow the player to tap into debug features

### Progression

Set your progress to any point in the game

### Camera

Allows to switch between 3 cameras: player chase camera, user camera and scene analyzer camera (the functionality of the last camera is still unknown).

#### Usage of user camera

* **T** - lock/unlock camera target to character 
* **I** - move camera forward  
* **K** - move camera backward
* **O** - move camera up 
* **U** - move camera down 
* **J** - move camera left 
* **L** - move camera right 
* **Right Shift** - speed up the camera 
* **Right Alt** - slow down camera 


## Extending functionality

If you desire to add any new option(s) to the menu, it's very simple.
`create_menu` is used to create a new menu.
`add_debug_menu_entry` is used to add entries to a menu.


## License

This project is MIT licensed
