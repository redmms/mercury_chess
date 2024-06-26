<p align="center">
   <img src="https://img.shields.io/github/last-commit/redmms/mercury_chess" alt="Last Update">
   <img src="https://img.shields.io/github/languages/code-size/redmms/mercury_chess" alt="Size">
</p>

#	MercuryChess: Toxic Edition.

Online chess with lots of stylesheets spiced with toxic humor. It uses an innovative data format in its built-in archiver that sets it apart from the rest.  
  
## Ready for production

This is not a project made just for the sake of it. It's primed for production. 
Actually, it consists of several different projects: server, gui chess, which uses archiver, which uses finestream. 
Archiver will be a closed project, I hope to make it commercial. 
Finestream is available here:  
https://github.com/redmms/finestream.git  
  
In the near future:  
![finestream_joke](/decription_media/finestream_joke.jpg)  
  
## Unique  

So, what is so special about MercuryChess? Sure, there are plenty of chess apps out there, but MercuryChess brings something fresh to the table. 
It's built entirely in C++, making it super snappy. Plus, it can potentially work on any device thanks to the Qt framework. 
It has a modern look and is made in harmonic colors. I tried to make it stylish. 
As if it wasn't enough, you can play with your friends online (or enemies, if you will make them play with you), chat, set profile pictures, set and get FEN notation, etc.  
  
But here's the real kicker: instead of the usual PGN format for saving games, MercuryChess uses its own compact format, saving you tons of space on your device. 
It compresses every half-move to less than 1 byte. 
PGN half-moves take about 5 bytes; other formats may use 12-16 bits per half-move; half-moves in my format take only 5 bits on average, 
so in the end it is more than 10x more compact compared to PGN.  
  
![format_comparison](/decription_media/format_comparison.jpg) 
  
## Friend offline mode

Whether you've got a buddy nearby or just want to challenge the strongest chess player (yourself), 
you can play in friend offline mode (Play->Offline->With friend). 
Use the mouse wheel to scroll the history area on the upper side of the board back and forward.  
  
![friend_offline_mode](/decription_media/friend_offline_mode.gif)  
  
## Training mode

Here (Play->Offline->Training) you can set any start position you want from a FEN string (the most popular format for describing board positions) to train your skills or test my chess engine. 
By default, it opens this position:  
https://www.chessprogramming.org/Perft_Results##Position_5   
  
![training_mode](/decription_media/training_mode.gif)  
  
## Friend online mode 

Set up your best profile picture (Settings->Profile and network), take some flowers, 
and call your beloved to play such an intellectual and developing game (Play->Online->With friend). 
Use the chat area on the right to say how much he or she matters to you.  
  
![friend_online_mode](/decription_media/friend_online_mode.gif)  
  
## History mode

At the moment, the archiver consists of two buttons: "Save game" and "Load game". 
To save a game, you need to first open the board and play some moves (in friend offline or friend online modes). 
To load a game in history mode, you need to choose a previously saved game in .mmd18 data format.  
  
![history_mode](/decription_media/history_mode.gif)  
  
## Constraints

Archiver is not available for training mode yet, though I will add it soon. 
std::map instead of QSettings is a temporary solution, so it does not save your profile picture and local nickname if you restart. 
The server is not supposed to be used for playing more than two players at the same time, it is a pet project, don't forget it. 
I don't get any money for this and spend my own money on running the server. 
If you like the project, consider buying me a coffee:  
https://ko-fi.com/mmd18   
https://www.buymeacoffee.com/mmd18  
It will inspire me to develop the project.   

## Build

Running a ready app:
1) Download the build from github Releases
2) Extract the file from the archive
3) Open game.exe
4) If some .dll files are missing open PRESS_ME shortcut, it will download VC++ redistributable packages installer, run it to install .dll files 

MS Visual Studio:
1) Clone the repository
2) Open qt.sln  
3) If it will not run, try to add /dxifcInlineFunctions- compiler command.

Qt Creator:
1) Clone the repository
2) Open all.pro
3) Choose Desktop Qt 5.15.2 MSVC2019 64bit kit

From scratch, without project file:  
  
If you're keen on diving into the code and building MercuryChess yourself, 
you will probably want to use MS Visual Studio (VS) with Qt VS tools addon to use QMake, because VS has the best C++20 module support at the moment.  
	You will need to enable Qt 5.15 if you want to hear sounds in the game (there's a bug in Qt 6):  
https://doc.qt.io/qtvstools/  
	And then you may need to use this compiler command: /dxifcInlineFunctions- because of this MSVC compiler bug:  
https://developercommunity.visualstudio.com/t/Modules-ICE-when-using-cout-inside-of/10299789?stateGroup=active&ftype=problem  
	Summary:  
Use MS VS 2022, Qt VS tools addon, enable Qt 5.15, enable C++20, MSVC v143 or later, add compiler command /dxifcInlineFunctions-  
