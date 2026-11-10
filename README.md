# Remote Input Receiver

A Windows C++17 UDP experiment that receives a small text protocol for mouse, keyboard and cursor commands. Network reception, packet validation and Win32 execution are separate so the protocol can be studied and tested without injecting input.

## Build

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\remote-receiver.exe --help
```

Requires Windows, C++17 and CMake 3.16+. With Ninja/MinGW-w64, the executable is directly in `build`.

## Run

```powershell
# Validate and report one packet on localhost; no input is applied.
.\build\Release\remote-receiver.exe --once

# Receive from a chosen interface and sender; explicitly enable input execution.
.\build\Release\remote-receiver.exe --bind 192.168.1.20 --peer 192.168.1.10 --apply
```

Default: `127.0.0.1:9009`, dry run. `--port` changes the port; zero chooses an ephemeral port and prints it. Ctrl+C stops the receive loop. `--once` exits after one packet from an allowed peer, returning 2 if that packet is invalid.

This is an unauthenticated, unencrypted UDP protocol for a controlled environment. An IP filter is not authentication. Delivery and ordering are not guaranteed; a missing key-up packet can leave a key held until shutdown. Do not expose an input-applying instance to untrusted networks.

## Three-field text protocol

Every datagram contains exactly `operation first second` as ASCII integers. One trailing NUL is accepted for historical senders. The text must fit 128 bytes; embedded NULs, missing fields, overflow and extra fields are rejected.

| Operation | First | Second |
| --- | --- | --- |
| 0 — mouse button | One Win32 flag: 2/4 left down/up, 8/16 right, 32/64 middle | 0 |
| 1 — key down | Virtual-key code 1–254 | 0 |
| 2 — key up | Virtual-key code 1–254 | 0 |
| 3 — move pointer | Signed screen X | Signed screen Y |
| 4 — vertical wheel | Signed delta, conventionally ±120 | 0 |
| 5 — restore system cursors | 0 | 0 |
| 6 — hide system cursors | 0 | 0 |

Example: `3 100 200`. This is the legacy **three-field text** format, not the seven-field format in Instruction-Receiver or a raw binary integer array.

## Code guide

- [command.cpp](command.cpp) / [command.hpp](command.hpp) — strict parsing and operation validation.
- [UDP_Receiver.cpp](UDP_Receiver.cpp) — owned Winsock/socket lifetime, exact packet sizes and a 200 ms idle timeout.
- [Message_Interpreter.cpp](Message_Interpreter.cpp) — checked Win32 execution and tracked state cleanup.
- [main.cpp](main.cpp) — explicit execution mode, bind/peer options and shutdown.
- [tests/receiver_test.cpp](tests/receiver_test.cpp) — parser regressions and loopback transport.

The previous unchecked function-pointer index, stale receive buffer, forced CPU affinity and explicit destructor call have been removed. On orderly shutdown, the interpreter attempts to release keys/buttons it pressed and restore cursors it hid. Process termination or Windows input restrictions can prevent cleanup. No shell commands are executed.

## Validation and limitations

Strict C++17 compilation and tests cover every opcode, malformed payloads, maximum-size UDP packets, short-after-long packets and idle timeout. Tests never call `SendInput`, move the pointer or replace system cursors. Interactive input, elevated applications and cursor restoration need testing in the intended desktop session.

## License

[MIT](LICENSE).
