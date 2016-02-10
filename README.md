# DDDAEx
A mod project for Capcom's game - Dragon's Dogma: Dark Arisen

## Roadmap

Targets for this project, followed by todo's.

###Injecting
Status: In progress
TODOs: Error-checking, memory-leak prevention

Current injection process:  
1. C++/CLI exports WinAPI functionality to C#.  
2. C# calls WinAPI to inject C++ dll into target process.  
3. C# calls WinAPI to inject C++ into itself (to get ExecuteCLR function offset).   
4. C# appends base module address in target process with function offset.  
5. C# calls CreateRemoteThread on target function in C++ dll, hosting the CLR.  
6. C++ calls static function in C# exe. C# code is now running in the target process.  

###Extensibility
Status: In progress
TODOs: Implement MEF2 so mods can be easily made with hot-reload in mind

