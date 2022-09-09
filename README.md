# Cortex - The psychedelic tunnel-runner

![Cortex](cortex.png)

This repo serves as an example project for multiplatform C, OpenGL and SDL2 development, and multi-store integration with free / open source tools. ( Vulkan transformation is planned ). 

I open sourced my beloved game to help those lost souls who are planning to release their games to every platform possible like me - and would like to avoid weeks of trial and error, head-scratching, stackoverflowing, documentation reading and pure sucking with different platforms and libraries and store APIs and in app purchases, in this repo you have everything and working.  

Cortex compiles for MacOS, Windows, Linux ( SteamOS, Ubuntu and Raspberry ), Android and iOS, is integrated with Steam ( Steam Inventory Service for donation purchase ), Google Play ( in-app billing for donations ) and the Apple App Store ( in-app purchases for donations ).

Check out Cortex on your preferred platform :

<a href="https://itunes.apple.com/az/app/cerebral-cortex/id725106153" target="_blank">for iOS - App Store link</a> 

<a href="https://play.google.com/store/apps/details?id=com.milgra.flow" target="_blank">for Android - Google Play link</a>

<a href="https://store.steampowered.com/app/998820/Cortex/" target="_blank">for Windows/MacOS/Linux - Steam Store link</a>

<a href="downloads/cortex/raspberry" target="_blank">for RaspBerry - Download</a> (  enable OpenGL / Full KMS Support in raspi-config )

Comment on Cortex/Message the author :

<a href="http://milgra.com/cortex.html">Cortex homepage</a>

If this project helped you, please donate from the game or through paypal at 

<a href="https://www.paypal.me/milgra">paypal.me/milgra</a>

---

## Project structure

framework - my own core library functions and tools. mt prefix stands for Milan Toth.

projects - projects and related files for all platforms, for sdl and for steam

sources - game source files and shaders

storage - differenct store related assets

---

## Compilation and Deployment guide

My primary development OS was MacOS. Development & Deployment on Linux shouldn't be a problem but on Windows Android development can be a pain ( actually it is a pain on all platforms but on windows it is a mega-pain ), and Linux/Unix file names can also be a problem when building steam depots for all platforms on Windows.

You will need an Apple Developer certificate for App Store integration and distribution and MacOS distribution, a Google Developer Account for Play Store integration and distribution and a Steam Developer account for Steam integration and distribution ( for Windows, Linux and MacOS ).


### MacOS project compilation and deployment

Tools : 

The preferred IDE is Xcode and actually that is your only choice if you want seamless signing & deployment. And it's free.
You will need git ( installing with homebrew is the easiest way https://brew.sh  ).

Project : 

The Xcode project file for the MacOS project is projects/sdl_macos/game_swarm.xcodeproject . 

The project is self-contained, doesn't use any external libraries.

SDL is added as source - all SDL header and source files are under "sdl" group.

Steam integration files are under "steam" group - it is one c++ class, it is called CInventory. 

The project uses one dynamic library : the libsteam_api.dylib, which is under the project folder and it is copied to the final bundle on build.

Preprocessor macros used in project : OSX and STEAM. Remove steam if you don't need steam integration.  Prepocessor macros are found in project settings -> preprocessor macros

Deployment :

For steam deployment you shold archive the project, export it with developer ID, sign it with your developer profile, export it to a folder somewhere on your machine, the final product is that.


### Linux project compilation and deployment

Tools : 

I used the latest Ubuntu because that's the most out-of-the box linux distro nowadays and GNOME is very MacOS like, more power to GNOME! :) As an IDE I used Code::Blocks because it's free, open source and quite lightweight ( compared to XCode and to Visual Studio ) ( but it needs a night to compile on a Raspberry PI :( ), and mainly becuase it will be our IDE on Windows and on Raspberry also so you only have to learn one IDE besides XCode. An other advantage of codeblocks that it maintains&compiles source file lists for you and you dont have to kill your soul with makefiles.

For Raspberry the latest Raspbian OS is used.

Git is needed as always. Actually git should be a kernel extension for all OSs :)

If you target SteamOS ( on Raspbian the following are unnecessary ) :

Schroot is needed. apt-get install schroot!

You have to checkout and build the steam-runtime ( https://github.com/ValveSoftware/steam-runtime ), which is a distribution-independent collection of linux libraries, and if you link your project against them then your game will run on all linux distros having steam or steam-runtime installed and it will run on SteamOS also. It will create a chroot folder on your machine for development & testing.

To link against the steam runtime from Code::Blocks a little trick has to be done : you have to schroot into the steam-runtime installed directory root ( schroot /home/../steam_runtime ), install Code::Blocks there ( sudo apt-get install codeblocks ) and run codeblocks from there so codeblocks will link againt the system libraries inside the chroot which are the steam-runtime libraries. The funny thing is that you cannot debug or run the game from inside the chroot for some reason I was lazy to go after, so if you want to test/debug then you have to start codeblocks outside chroot and link directly against the steam runtime libs. 

Project : 

The project is self-contained, doesn't use any external libraries.

Steam integration files are under "steam" group - it is one c++ class, it is called CInventory. 

The Code::Blocks project file for the Ubuntu project is projects/sdl_ubuntu/sdl_linux.cbp . 
The Code::Blocks project file for the Raspberry project is projects/sdl_pi/sdl_linux.cbp . 

Deployment : 

Run a release build, final product is undr sdl_ubuntu/bin/Release


### Windows project compilation and deployment

Tools :

Code::Blocks is needed for IDE. You should download the MinGW version. MinGW is a GNU compiler & library collection for Windows and with it CodeBlocks can build a microsoft-lib-independent Win32 binary. I used the 64 bit version ( https://mingw-w64.org/doku.php ) because the steam client is 64 bit and my 32-bit game version crashed during IPC communication with the steam Client :) And 64 bit is common nowadays. 

Since we use the MinGW libraries we have to ship them with the game so the game on windows is quite self-contained.

GitBash is also mandatory : not only gives it git to you but it gives a bash terminal with unix commands, super cool!

Project : 

The project is self-contained, doesn't use any external libraries.

Steam integration files are under "steam" group - it is one c++ class, it is called CInventory. 

The Code::Blocks project file for the Windows project is projects/sdl_ubuntu/sdl_win.cbp .

Deployment : 

Run a release build, final product is under sdl_win/bin/Release


### iOS compilation and deployment

Tools :

Xcode, git

Project :

The Xcode project file for the iOS project is projects/sdl_ios/game_swarm.xcodeproj .
See the MacOS Xcode project for reference, it is almost the same but without steam integration.

Deployment :

Archive the project and upload it to the App Store.


### Android compilation

Tools :

Android Studio is the preferred IDE. You will also need the android ndk. Git is elementary.

Project : 

projects/sdl_droid is the project folder.

You have to create four symlinks under projects/sdl_droid/app/jni if git doesn't checks it out for you.

Link SDL2's src and include folder under projects/sdl_droid/app/jni/SDL2

Link the project's framework and source folder under projects/sdl_droid/app/jni/src   

Deployment :

Select release build variant, then generate signed APK from Build menu, then locate.


### Steam Store Setup

There are multiple ways to implement donations in steam and I chose the Steam Inventory Service because of it's simplicity. To enable it, go to your game's SteamWorks admin, Community tab -> Inventory Service. Enable inventroy service with the checkbox and at the Item Definitons part upload your item definiton file. For Cortex it is projects/steam/Cortex_items.json . After this you are able to reach/buy your items from the game through the SteamWorks API. 


### App Store Setup

I used the consumable product category for donation. Go to your app in App Store Connect, Features tab, In-App Purchases, add your consumable items. Product id's must be unique for all your apps. After you saved them they you can use/test them within your app, but before release you have to add them to your app.


### Play Store Setup

In Play Console go to your App -> Store Presence tab -> In-app products , add your Managed Products. After this you can use them/test your app. 


### SteamWorks Deployment

Download and unzip SteamWorkds SDK on your preferred location. Under tools/contentBuilder/content a folder for the game depots have to be created. In our case it is Cortex. Under it there is a folder for MacOS, Windows and Linux steampipe depots, game files have to be placed here.

Place your scripts under tools/contentBuilder/scripts, one for the app and one for each steampipe depot. For Cortex all scripts are under projects/steam/deploy, I also created a shell script ( build.sh ) that automates logging in and upload, add your username and password into the script.

---

## Testing

I used VMWare to virtualize Windows, Linux and SteamOS on my MacBook. Your other option is Parallels because VirtualBox can only emulate OpenGL 1.0 which is ancient and you won't develop games for it in the 3rd millennia. Your free option is dual-boot or to use a dedicated machine. I used a shared folder to exchange data between the MacOS host and the linux guest. Mainly for sending the compiled game files to the host to pack them together with SteamWorks SDK, but you can deploy depots independently from the corresponding platforms if you are brave enough. 

You have to install Steam on all platforms ( besides SteamOS ) and the Steam client must run to make in-app purchases work.

Windows and Ubuntu should install seamlessly in VMWare but there was a problem with SteamOS - by default it tries to use the NVidia video drivers and it fails. So you have to CTRL+ALT+FX to a working terminal, log in with the default user ( desktop ), and delete NVidia drivers from the system. After a restart it should work with the default drivers, if not, try to install VMWare tools or Mesa drivers.

For Raspberry you need a real device, a model with WiFi is the best.

For iOS/Android testing the emulator is good for dev-time testing, but for in-app purchases and gameplay you need to get a device temporarily.

When you test your steam distributed game, always check the game files on the filesystem. You can mess up depots badly, or an older save file can also mess with the functionality of your program, steam doesn't delete new files when you uninstall an app.

--

## Understaning the game code

The game is written in C. I got really tired of programming in the last 18 years so I went for the least code and least files possible but with maximum readability and structure. How did I achieve this?

- headerless C. Maintaining header files is a pain in the ass and so 70's. C can do it headerlessly too (https://github.com/milgra/headerlessc) 
- reference counted memory management. mtmem_alloc does the trick, mt_* types handle retain/release counting internally.
- new-line filled airy code - this forces you to write short functions that do only one thing.
- modular programming with mediator - implementation groups with different functionality don't include each other directly, they send messages to each other
- if a module is "static" a global variable is used to store it's state, instancing is meaningless 
- no "initializer" methods if not needed, structs are defined inline and raised to the heap with a mtmem macro
- instead of platform #define's platform-related stand-alone implementation files are used where possible

Altough everything is in C, the main file is cpp. How come? Because steam API is c++ only that's why main.c had to be renamed to main.cpp and all main functions had to be 'extern "C"'-d. It's super because instead of three lines in C the integration had to be done with two additional files, macro magic and c->c++ bridging.

Shader trick. As you see the shader files are not valid shader files, they are C strings. That's beacuse loading shaders runtime is slow and cumbersome, I let the compiler do that with simple #includes. And they are still easy to modify this way. 

### Files

main.cpp - SDL handling, top level event handling

ui.c - ui handling rendering ( main module )

scene.c - scene handling and rendering ( main module )

renderer.c - rendering ( main module )

--

## Tips and tricks

Android Studio :

- if a native file is not appearing/not added to your project select refresh linked c++ projects from the menu
- if uploading an apk to the device takes minutes switch off "Instant Run" in preferences

Android ndk dev :

- if your char string doesn't end with a \0 you will have a bad time!
- always build the native part manually from the command line with "ndk-build -B V=1" It will print out everything like gcc, this is your only chance to catch include problems with ndk builds which you will face a lot!

Main Development Machine :

- Create a DevTools folder, put in the vmware images for Windows, Linux and SteamOS, Android sdk and ndk, SteamPipe and SDL so it is easier to backup all stuff for a reinstall.

Xcode iOS/Mac :

- If you would like to create an empty project with the SDL source in it then add the full sdl source/header list to the project, and compare it with SDL's own MacOS project and delete the unnecessary folders from your project's sdl folder.
- Always check for unnecessary items in your release build, it's easy to add a file to the project and leave the "add to project" checkbox checked so it will be copied to the final product 

Linux :

- Figuring out what shared library does a binary/library use and where they are and which version is included is a nightmare. Your best friend is ldd and locate. The most annoying part that relative search paths are somehow written in the binary by the linker and you have to use magic macros to force the linker to link to your desired library! For example libsteam_api is linked to the product's folder with this linker option : "Wl,-rpath=\\$$$ORIGIN"

CodeBlocks :

Setting up compiler and linker include paths is a vital part for CodeBlocks/C compilation. Always check everything in project/build options. Always select the full release, the most common mistake is to set things for Debug or Release target only and you have no idea why it isn't working. Check compiler include paths, linker include paths for both targets carefully, check compiler and linker options also carefully. 

SteamWorks :

When uploading new inventory item descriptions be sure to validate it with an online json validator because steam won't tell you much about its problems - it just rejects the file. Good practice to start with the simplest file - one item and the unnecessary elements - and extend it if steam accepts is.

If Invetory Store stops working for testers, try to disabe/enable the steam inventory store for your app, it worked for me.

--

## My humble opinion on platforms

First of all, I did all development on an early 2016 12'' MacBook with a fanless intel core m7, 8GB RAM and 512GB SDD and it kicked ass! It runs Windows and Linux smoothly in VMWare, the game ran with 60 fps inside the virtual machine. XCode/CodeBlocks building is also superfast. 

What kills it is Android Studio. I don't think CPU development will ever reach a state where Java desktop applications run smoothly. 
And it is not only slow because of Java, slowness is amplified by the gradle-scripts that run between the IDE and the project so there is a very loose connection between the IDE and the code. Actually my general feeling of Android development is that there is a very loose connection between everything and you don't know what is really happening and why is it happening. Learning curve is super steep. I can imagine developers who gave 5-6 years of their lives to android development and have a mostly clear picture on whats and whys but I'm not planning to be one :) Anyway, great respect to android developers, it seems to be the biggest suck factor in the industry nowadays.

To be a decent desktop operating system Linux needs a default GUI and a simple way to install binary/closed source applications. GNOME is okay but all developers should stand behind it and push it together towards perfection, and a bundle-based application structure would be awesome ( like on MacOS ) without dependence magic. 
For open-source programs apt-get install is fun until you have to add new sources to the sources list or older versions with removed dependencies, etc. Compiling from source is also fun, for sysadmins and time-millionares :)

iOS and it's API's became way too complicated. Doing autolayout in Interface Builder is a lifelong journey, doing things that were super simple back in 2010 are now super-complicated ( hiding the status bar, rotation, etc ), entitlements files are everywhere for increased security. The biggest pain was an fopen issue, it worked a few years earlier but now it only creates the file and then it cannot be read/written. It turned out that fopen on iOS creates files with 0000 permissions instead of 0666 which caused a 2-hour head scratching. Using open with explicit permissions solves the problem but why did fopen became obsolete?

Raspberry is a super cool little machine. It was super easy to port the game to it, runs well, I love it.

Steamworks is a mess. The API is a mess and the site is a mess. I spent days clicking through the site and I still have no idea how to go to the steamworks admin/store admin/the main page of the application with three clicks, I think it's impossible. Settings are scattered everywhere and the whole thing is backed by Perforce!!! You have to publish your changes every time to Perforce, it's insane. It's like a high school project. The documentation is not really talkative, I used the Steamworks sample project, the documentation and google together to fix issues but I wasn't prepared for random persistence errors which can be solved by disabling and re-enabling Inventory Service for example. But they are the biggest, have infinite money, they can do this :)

The best OS for multiplatform development is definitely MacOS. It puts everything under your ass out of the box and then gets out of your way. It has everything that linux has and everything that windows has and much much more.
