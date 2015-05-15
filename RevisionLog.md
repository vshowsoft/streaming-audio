# A Note About Version Numbers #
The streaming-audio project uses a major.minor.patch version numbering system.  Releases have even-numbered minor versions, and development (potentially unstable) versions have odd-numbered minor versions.  Patches are made to release versions to fix bugs, and releases with the same minor version number are always compatible (eg a 0.4.2 client can stream to a 0.4.1 SAM).

# Revision History #

## Version 0.6.0 ##

  * Enhanced SAM/client communication:
    * Clients can now request a packet queue size that is different from SAM's default.  The length of the packet queue defines how many packets are buffered in SAM before playback and therefore affects latency.
  * Improved client library API:
    * Removed redundant and confusing constructor (use default constructor + init()).  Clients not using the removed constructor should be able to build with the new library without changes.
    * Users can register callbacks to be notified when the client loses its connection to SAM or the client is muted or soloed.
    * Users can drive audio streaming with an external clock (ie Pd or Max) rather than using JACK to drive the client's audio callback mechanism.
    * Introduced a `SacParams` struct of client parameters which can be used to initialize a client without passing large numbers of individual parameters.
    * Clients can now bind to a specific network interface instead of all interfaces on machines with multiple interfaces (supported in Qt5 only).
    * Clients can now request a particular payload type (16-bit, 24-bit, or 32-bit).
  * Enhanced SAM/Renderer communication:
    * Added a `preset` parameter to accompany rendering `type`.
    * Rendering types are no longer hard-coded in SAM but are instead defined dynamically by the renderer.
    * Added check for compatible SAM/renderer versions.
  * Configuration file parameter changes:
    * Specify the default packet queue size with `PacketQueueSize`.
    * Specify the clock skew compensation threshold with `ClockSkewThreshold`.
    * Force checking for matching patch version in addition to major/minor version check with `VerifyPatchVersion`.
    * `MeterIntervalMillis` defines how often OSC meter messages should be sent to subscribers and also how often the local UI's meters will update.
    * Specify the maximum delay for clients (`MaxClientDelayMillis`) separate from the maximum global delay (`MaxDelayMillis`).
    * `OutputJackClientNameBasic` and `OutputJackPortBaseBasic` allow routing basic channel outputs to JACK clients other than just system:playback\_N etc.
    * `OutputJackClientNameDiscrete` and `OutputJackPortBaseDiscrete` allow routing discrete channel outputs to JACK clients other than just system:playback\_N etc.
    * `OutputPortOffset` was removed.
    * `HostAddress` tells SAM to listen for OSC messages only at the specified IP on machines with multiple interfaces.
    * `MaxClients` specifies the maximum number of concurrent client connections allowed.
  * Enhanced SAM/UI communication
    * Rendering types and presets are defined dynamically by renderer and forwarded on to UI from SAM.
    * Global and per-client max delays are returned in regconfirm message.
    * Added check for compatible SAM/UI versions.
  * Alpha release of a simple local UI for SAM that allows muting, soloing, and changing volume or delay of individual clients.  Primitive metering is included.
  * New clients available:
    * Simple GUI version of samugen example
    * Externals for Pd and Max
    * Mplayer audio output plugin
  * Miscellaneous:
    * Various bug fixes.
    * All streaming-audio code is now in the `sam` namespace.
    * Eliminated use of Qt functions that were deprecated in Qt5.  SAM and clients can now use either Qt4 or Qt5.

## Version 0.4.2 ##
  * Improved configure script that now works on Linux Mint

## Version 0.4.1 ##
  * Fixed bug with default client timeout for connecting to SAM.

## Version 0.4.0 ##