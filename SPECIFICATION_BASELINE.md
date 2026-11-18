# Wicofe Userspace ABI Specification Baseline

- **Status:** Draft; not stable
- **Version:** 0.1
- **Owner:** Wicofe-ABI
- **Implementations:** Wicofe-Kernel, Wlibc, Wicofe-Lib, Wicofe-SDK

## 1. Scope

This repository is the canonical source for the public interface between Wicofe user processes and the kernel. It owns syscall identifiers, calling conventions, user-visible structures, constants, error semantics, and compatibility policy.

The Dynamic Kernel Module ABI is not defined here. DKM is a privileged in-kernel interface owned by Wicofe-Kernel and documented separately.

The userspace ABI must support the native ISO C/POSIX runtime contract documented in the [Unix userspace foundation specification](https://github.com/Waizusora/Wicofe-Specs/blob/main/userspace/UNIX_USERSPACE_FOUNDATION_0.1.md). POSIX library interfaces are implemented by Wlibc; they are not automatically one-to-one with syscalls.

## 2. Design principles

- Architecture-aware, but architecture-neutral wherever possible.
- Explicit fixed-width types and C-compatible data layouts.
- No dependency on Rust compiler-private ABI or type layout.
- Validate all user pointers, lengths, offsets, and flags in the kernel.
- Version every public interface and document its stability.
- Preserve compatibility once an interface is declared stable.
- Generate headers/bindings from canonical definitions where practical to prevent drift.
- Specify semantics needed by the declared POSIX profile without importing host-kernel ABI assumptions.

## 3. Planned interface domains

The initial ABI is expected to cover these domains in dependency order:

1. Process and thread lifecycle.
2. Memory mapping and address-space operations.
3. File descriptors and filesystem operations.
4. Time, clocks, and timers.
5. Synchronization and IPC.
6. Signals and process notification.
7. Sockets and networking.
8. Identity, permissions, and resource limits.
9. System information and capability discovery.

This is a scope outline, not a promise that every listed interface will be part of the first ABI release. Some POSIX interfaces are implemented in userspace using multiple syscalls or library algorithms.

## 4. Per-call specification template

Every proposed syscall must document:

| Field | Required content |
|---|---|
| Name and number | Unique stable identifier; number remains unassigned until reviewed |
| Availability | Architecture and minimum ABI version |
| Arguments | Order, type, width, signedness, alignment, pointer direction |
| Return | Success value and raw failure representation |
| Errors | Defined error conditions and libc mapping |
| Blocking | Whether it may block, be interrupted, or be restarted |
| Permissions | Required identity, capability, or object rights |
| Side effects | State changes and partial-completion behavior |
| Stability | Draft, experimental, stable, or deprecated |
| Tests | Positive, negative, boundary, and compatibility cases |

## 5. Data representation rules

Public structures must use explicit-width integer fields and documented alignment. Avoid implicit padding assumptions; where layout matters, test size, alignment, and field offsets for every supported target.

Pointers are process virtual addresses and must be represented/documented as such. A pointer's ownership, mutability, lifetime, nullability, and permitted range must be specified.

Reserved fields must have explicit rules. Callers should initialize reserved input fields to zero; the kernel must not interpret them unless a later ABI version defines them. Output reserved fields must be initialized as specified by the individual interface.

Do not pass Rust `String`, `Vec`, references, slices, trait objects, or compiler-layout enums across the ABI.

## 6. Error convention

The raw syscall error convention is not yet selected. The specification must choose one convention and define the complete valid success range so that errors cannot be confused with successful return values.

The libc layer will translate raw kernel errors into the C `errno` contract. The kernel ABI and libc wrapper must not independently invent error numbers or mappings.

## 7. Architecture policy

The initial target architectures are x86-64 and AArch64. The ABI must state the syscall-entry mechanism and register assignment for each architecture while preserving the same logical syscall semantics.

Architecture-specific register details must not leak into portable userspace headers. Provide architecture-specific low-level stubs or generated bindings as needed.

## 8. Versioning and stability

The ABI is currently draft and is not stable. Each interface must carry an introduction version and stability label. Breaking changes to stable interfaces require an explicit compatibility plan and architecture decision record.

Kernel and userspace components must negotiate or expose supported ABI capabilities where version equality alone is insufficient.

## 9. Initial exclusions

The following are not part of this repository's userspace ABI:

- DKM metadata and kernel module loading ABI.
- Private in-kernel Rust interfaces.
- ONWFS on-disk format.
- Userspace service protocols unless explicitly promoted to a system-wide public ABI.
- GNU/BSD library extension APIs; Wicofe's C runtime target is the declared ISO C/POSIX profile.

**No syscall numbers or register assignments are frozen by this baseline.**
