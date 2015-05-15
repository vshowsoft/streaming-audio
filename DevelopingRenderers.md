# SAM Renderers #


The current release version is 0.6.
The following information applies to the current trunk (version 0.7.1 and above).

## Rendering Background ##
Renderers provide advanced rendering modes to extend SAM functionality. For example, when using SAM to provide audio for large-scale tiled display environments such as SAGE, a renderer can be used to spatially align audio with a client's visual image.

Internally, SAM understands two types of output: basic and discrete.  Clients using basic output (always rendering type 0) are all mixed down to the basic output channels specified in the SAM config file (typically two channels).  Clients using discrete output (non-zero rendering type) are allocated output channels and sent discretely to the renderer with no mixing in SAM.
Only one renderer may be registered with SAM at any given time.  However, that renderer is allowed to support multiple rendering modes/types.

### Connecting SAM and a Renderer ###
The renderer may run on a separate machine from SAM, in which case the two machines must be connected by physical audio interfaces and cables, and the number of clients which can use discrete rendering is limited by the number of physical channels connecting the two machines.  Alternatively, the renderer may run on the same machine as SAM.  In this case, routing between SAM and the renderer is performed using JACK.  The renderer should be running before SAM in this case, and the SAM config file must specify the renderer's JACK client name and how it names its ports.

## The Rendering Library ##

[Doxygen-generated documentation of samrender library (for trunk)](http://streaming-audio.googlecode.com/svn/trunk/doc/html/index.html)

A rendering library is now available to facilitate the development of third-party renderers for SAM.  This library encapsulates the OSC message-based communication between SAM and the renderer.  OSC messages are exchanged using a TCP connection which is maintained as long as the renderer is registered with SAM.  This connection allows both SAM and the renderer to know if the other goes offline.

### Example Renderer ###
An example SAM renderer is available in the src/render/examples directory.

### Dependencies ###
The `samrender` library uses Qt 4, version 4.7 or higher, although it should build successfully against Qt 5 as well.  The default library build links dynamically against the Qt libraries, so rendering applications must also link against them.  If using qmake, the .pro files for the example renderer described above is a good reference.

To account for these dependencies, if you are writing your own makefile, you will probably need to add a variation of the following for headers:
```
-I/usr/include/QtCore -I/usr/include/QtNetwork -I$(SAM_ROOT)/src -I$(SAM_ROOT)/src/render
```

and a variation of the following for the linker:
```
-L/usr/lib64/qt48 -lQtNetwork -lQtCore -L$(SAM_ROOT)/lib -lsamrender
```

### Return/Error Codes ###

`SamRenderer` methods return one of the following codes:
| SAMRENDER\_SUCCESS | 0 | Success |
|:-------------------|:--|:--------|
| SAMRENDER\_REQUEST\_DENIED | 1 | A SAM request was denied (i.e. registration or changing type failed) |
| SAMRENDER\_NOT\_REGISTERED | 2 | Attempted to send a request to SAM before registering |
| SAMRENDER\_OSC\_ERROR | 3 | An error occurred trying to send an OSC message to SAM or receive one from SAM |
| SAMRENDER\_TIMEOUT | 4 | A request to SAM timed out waiting for a response |
| SAMRENDER\_ERROR | 5 | An error occurred that doesn't fit one of the above codes |

### Initializing a Renderer ###
To initialize a renderer, you must provide parameters using a `SamRenderParams` struct which contains the IP address of the SAM instance this client will connect to, the port on which the SAM instance is listening for OSC messages, and the IP address and port where which the renderer will listen for reply OSC messages.
```
#include "samrenderer.h"
using namespace sam;

SamRenderParams params;
params.samIP = "127.0.0.1";
params.samPort = 7770;
params.replyIP = "127.0.0.1";
params.replyPort = 7771;

SamRenderer renderer;
if (renderer.init(params) != SAMRENDER_SUCCESS)
{
    printf("Couldn't initialize SamRenderer");
}
```

Note that `SamRenderer` is in the `sam` namespace.

### Starting a Renderer ###
After successfully initializing the renderer, the renderer must be started:
```
if (renderer.start(timeout) != SAMRENDER_SUCCESS)
{
    printf("Couldn't start renderer");
}

```

Starting the renderer initiates the process of registering with SAM using OSC over TCP.  The method then blocks waiting for a response from SAM.  You can specify a timeout (in milliseconds) for the blocking/waiting, after which point `start` will return an error code indicating that it has timed out and did not receive a response from SAM.  If starting was successful, a TCP connection between the renderer and SAM remains in place for the exchange of OSC messages until the renderer is stopped.

### Stopping a Renderer ###
To stop a renderer, simply destroy the `SamRenderer` object.  This will cause the renderer to unregister with SAM and clean itself up neatly.  If your application crashes and the `SamRenderer` destructor is never called, your renderer may remain registered with SAM.  However, SAM should detect that its TCP connection with the renderer has been disconnected and correctly unregister the renderer.

### Rendering Types and Presets ###
SAM internally supports basic rendering (type 0) with a default preset (preset 0), and streams with type 0 are never sent to the renderer.  To receive streams, the renderer must tell SAM what types (and associated presets) it supports.  For example, `TestRenderer` supports one type (type 1 with name "Mono Mix-Down"), with one preset (preset 0 with name "Default").
```
int presetIds[] = {0};
const char* presetNames[1] = {"Default"};
renderer.addType(1, "Mono Mix-Down", 1, presetIds, presetNames);
```

Until rendering types have been added by the renderer, no clients can choose an advanced rendering type.

Note that each rendering type must support at least one preset (the default).

### Callbacks ###

#### Stream Added/Removed ####
When a new client registers with SAM, SAM notifies the renderer and tells the renderer what type of rendering the client has requested and what output channels SAM has allocated for that client (note that the client's allocated channels can be non-consecutive).  Your renderer must know how SAM's output channels are connected to its own input channels so it knows what input channels that client's audio will arrive on.  Your renderer is then responsible for rendering the client audio in the requested manner.

To be notified when a client (stream) has been added, you must write a
`StreamAddedCallback` (example from `TestRenderer`):
```
void TestRenderer::stream_added_callback(SamRenderStream& stream, void* renderer)
{
    ((TestRenderer*)renderer)->addStream(stream);
}
```
and register that callback:
```
renderer.setStreamAddedCallback(stream_added_callback, this);
```

Similarly, to be notified when a stream has been removed, you must write a `StreamRemovedCallback` (example from `TestRenderer`):
```
void TestRenderer::stream_removed_callback(int id, void* renderer)
{
    ((TestRenderer*)renderer)->removeStream(id);
}
```
and register that callback:
```
renderer.setStreamRemovedCallback(stream_removed_callback, this);
```

#### Type ####
To be notified when a stream's type or preset changes, you must write a `RenderTypeCallback` (example from `TestRenderer`):
```
void TestRenderer::type_changed_callback(int id, int type, int preset, void* renderer)
{
    ((TestRenderer*)renderer)->changeType(id, type, preset);
}
```
and register that callback:
```
renderer.setTypeCallback(type_changed_callback, this);
```

#### Position ####
To be notified when a stream's position changes, you must write a `RenderPositionCallback` (example from `TestRenderer`):
```
void TestRenderer::position_changed_callback(int id, int x, int y, int width, int height, int depth, void* renderer)
{
    ((TestRenderer*)renderer)->changePosition(id, x, y, width, height, depth);
}

```
and register that callback:
```
renderer.setPositionCallback(position_changed_callback, this);
```

#### Lost Connection ####
To be notified when the TCP connection with SAM is broken, you must write a `RenderDisconnectCallback` (example from `TestRenderer`):
```
void TestRenderer::render_disconnect_callback(void*)
{
    qWarning("TestRenderer::render_disconnect_callback lost connection with SAM, shutting down...");
    QCoreApplication::quit();
}
```
and register that callback:
```
renderer.setDisconnectCallback(render_disconnect_callback, this);
```