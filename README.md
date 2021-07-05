# DDDAEx
A mod project for Capcom's game - Dragon's Dogma: Dark Arisen

## Roadmap

Targets for this project, followed by todo's.

### Injecting
**Status**: In progress (Low priority)  
**Tasks**
- [x] Use CreateProcess with Suspended to load CLR ASAP.
- [ ] Error/sanity checking.
  - [ ] CLR not installed.
  - [ ] Not enough access rights to inject.
- [ ] Resolve memory-leaks while injecting.

**Current injection method**
- CreateProcess (this starts Steam to prepare the process).
- Wait for Steam process to exit.
- Capture game process and suspend it with OpenProcess.
- Write infinite loop into memory where main thread is to wait for DLLs to load.
- Restore code and call LoadLibrary to load the CLR.
- Wait for process to initialize a bit.
- CreateRemoteThread on ExecuteCLR function in injected DLL.

### Hooking
**Status**: In progress (High priority)  
**Tasks**  
- [ ] Inject DirectX's interface and bind it to the SharpDX framework (take a look at EasyHook).
  - [ ] Hooking EndDraw scene.

Keep in mind to allow other mods to easily extend it.

### Extensibility
**Status**: In progress  
**Tasks**  
- [ ] Implement MEF2 so mods can be easily made with hot-reload in mind
  - [ ] Init and Dispose methods for mods to fill in.
- [ ] Add a data-storage location for mods to easily read from (less error-prone this way).

**Current process idea**  
Allow .NET dlls to export classes through MEF 2.  
Classes for: Drawing hooks and Logic hooks.  

### Networking
**Status**: Todo  
**Tasks**  
- [ ] Create a protobuf serialized message system over async server->clients topology.
- [ ] Use minimal data per message (auto increment message id per registered message).
- [ ] Force mods to use type-based equality checking for packets (never on IDs!).
- [ ] Allow mods to register their own packets through MEF 2.
- [ ] Allow mods to bind logic for packets.
  - (Example) OnIncomming: Write changes to memory locations.
  - (Example) OnOutgoing: Read changes to memory and send to server/client.
