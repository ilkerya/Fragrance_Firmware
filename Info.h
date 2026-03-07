/* 
For Sending Commands Set-> New Line / 115200 baud
Commands List  // CaseSensitive , only one emptySpace 
FanH XX  //16-98 as %Duty cycle
FanM XX    //16-98
FanL XX    //16-98
ColH XXX  //0-255  from Colorpalette
ColM XXX   //0-255
ColL XXX   //0-255
Reset   // System Reset
Sleep  // Goes to sleep wake up by key touch

Panel Explanation
%FSet:LowSpeed/MidSpeed/HighSpeed
CSet:LowColor/MidColor/HighColor

for clean compilation of the project, delete the folder:
C:\Users\XXXX\AppData\Local\arduino\sketches

//  https://www.ditig.com/256-colors-cheat-sheet
/*
First Time
git init
git add README.md
git add Branch main

GitHub Bash Commands
Adding new file
https://help.github.com/articles/adding-a-file-to-a-repository-using-the-command-line/#platform-windows

git add .
git commit -m "Add existing file"
// For Common git
git push https://github.com/ilkerya/Fragrance_Firmware main/sub

git remote add origin <remote repository URL>
git remote add origin https://github.com/ilkerya/Fragrance_Firmware

In case in the first push gives error use below command
git push origin main
// in case problem use --force option
git push --force https://github.com/ilkerya/Fragrance_Firmware main/sub --force

To create a branch:
$ git branch <branch_name>
To switch to that branch:
$ git checkout <same_branch_name>
To do the above operations in one line, it will create and switch branch:
$ git checkout -b <new_branch>
For example, say you want to create a branch named `learning_git`
$ git branch learning_git
$ git checkout learning_git
or
$ git checkout -b learning_git

C:\Users\XXX\OneDrive - XXX\Documents\Arduino\Fragrance> // path
C:\Users\XXX\OneDrive\ -\ XXX\Documents\Arduino\Fragrance>  // command line

cd OneDrive\ -\ XXX

Free PDF Editor for schematics

https://www.sejda.com/





























*/