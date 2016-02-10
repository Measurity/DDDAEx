# DDDAEx
A mod project for Capcom's game - Dragon's Dogma: Dark Arisen

## Roadmap

Targets for this project, followed by todo's.

###Injecting
**Status**: In progress  
**TODOs**: Error-checking, memory-leak prevention  

**Current injection process**
- C++/CLI exports WinAPI functionality to C#.
- C# calls WinAPI to inject C++ dll into target process.
- C# calls WinAPI to inject C++ into itself (to get ExecuteCLR function offset).
- C# appends base module address in target process with function offset.
- C# calls CreateRemoteThread on target function in C++ dll, hosting the CLR.
- C++ calls static function in C# exe. C# (.NET) code is now running in the target process.

###Hooking
**Status**: Todo  
**TODOs**  
- Inject DirectX's interface and bind it to the SharpDX framework (take a look at EasyHook).

Keep in mind to allow other mods to easily extend it.

###Extensibility
**Status**: In progress  
**TODOs**  
- Implement MEF2 so mods can be easily made with hot-reload in mind

**Current process idea**  
Allow .NET dlls to export classes through MEF 2.  
Classes for: Drawing hooks and Logic hooks.  

###Networking
**Status**: Todo  
**TODOs**  
- Create a protobuf serialized message system over async server->clients topology.
- Use minimal data per message (auto increment message id per registered message).
- Force mods to use type-based equality checking for messages (never on IDs!).
- Allow mods to register their own messages through MEF 2.
