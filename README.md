# Wicofe ABI

The canonical interface contract between the Wicofe kernel and userspace.

## Scope

- System call identifiers, calling convention, arguments, and return/error semantics.
- User/kernel data structures, alignment, size, and versioning rules.
- Process, thread, memory, file descriptor, signal, and time interfaces.
- ABI compatibility policy and generated bindings where appropriate.
- POSIX-facing behavior required by Wlibc, without importing host-kernel ABI assumptions.

## Ownership boundaries

This repository defines the contract; Wicofe-Kernel implements kernel-side behavior, while Wlibc and Wicofe-Lib provide userspace interfaces. Do not duplicate or independently renumber syscall definitions in dependent repositories.

## Wlibc and POSIX runtime consumer

[Wlibc](https://github.com/Waizusora/WLibc) is the native C library and POSIX runtime project. It is planned to fork musl and adapt its runtime to this repository's native Wicofe syscall ABI. Wlibc targets ISO C17 and IEEE Std 1003.1-2024 (POSIX.1-2024 / Issue 8); GNU and BSD compatibility extensions are not project requirements. Any separate POSIX layer must consume this same ABI and must not define competing syscall numbers, structures, or error conventions.

## Frenit integration consumer

[Frenit (Free Init)](https://github.com/Waizusora/Frenit) is a planned userspace consumer of process, exec, wait/reap, signal, descriptor, clock, IPC and shutdown interfaces. This repository remains the sole owner of their public syscall numbers, calling convention, data layouts and error semantics. Frenit and Wicofe-Kernel must not invent parallel syscall definitions or assume Linux binary ABI compatibility. Native Frenit PID 1 integration remains blocked until the relevant ABI entries are specified, reviewed and implemented.

## Specification

- [Userspace ABI specification baseline](SPECIFICATION_BASELINE.md)
- [Unix userspace foundation](https://github.com/Waizusora/Wicofe-Specs/blob/main/userspace/UNIX_USERSPACE_FOUNDATION_0.1.md)

## Compatibility rules

Every public interface must document stability level, version introduction, reserved fields, error behavior, and migration policy. Never expose Rust compiler-private layout as a stable ABI; use explicit C-compatible representations where required.

## Status

Initial repository baseline. The ABI is not yet declared stable. Proposed interfaces must be reviewed before implementation dependencies rely on them.

## License

Original project material is intended for MPL-2.0. Third-party specifications and copied material retain their own terms and attribution requirements.

## Documentation

See [docs/INDEX.md](docs/INDEX.md) for technical documentation. The [central repository documentation map](https://github.com/Waizusora/Wicofe-Docs/blob/main/REPOSITORY_DOCUMENTATION_MAP.md) identifies canonical ownership and cross-repository boundaries.
