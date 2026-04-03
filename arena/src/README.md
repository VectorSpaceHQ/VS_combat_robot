# Clock Setup

There are four transmitters: P1, P2, MC, and Judge.
There is one clock.

The four transmitters are all loaded with the same code: remote.ino. After uploading the code, each has to be configured through the serial monitor using the Prefs library. This is where the clock's MAC address is set and the remote's name.

# Prefs configuration
Open the serial monitor.
Set the clock's mac address with,

    setp remote address Bytes 4827E23D0174

Set the remote's identity as P1 (1), P2 (2), Judge (3), or MC (4)

    setp remote sender UInt8 3



# Check configuration

  getp remote address Bytes