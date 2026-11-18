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
- Generate headers/bindings from canonical definitions where practical.
- Specify semantics needed by the declared POSIX profile without importing host-kernel ABI assumptions.

## 3. Planned interface domains

The initial C runtime target is ISO C17 and IEEE Std 1003.1-2024 (POSIX.1-2024). Wlibc-specific kernel capability requirements are enumerated in [Wlibc Native ABI Requirements 0.1](docs/WLIBC_ABI_REQUIREMENTS_0.1.md).

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

Every proposed syscall must document its name and number, availability, arguments, return values, errors, blocking/interruption behavior, permissions, side effects, stability and tests. Numeric identifiers remain unassigned until reviewed.

## 5. Data representation rules

Public structures must use explicit-width integer fields and documented alignment. Avoid implicit padding assumptions; where layout matters, test size, alignment, and field offsets for every supported target.

Pointers are process virtual addresses and must be represented/documented as such. A pointer's ownership, mutability, lifetime, nullability, and permitted range must be specified.

Reserved fields must have explicit rules. Callers should initialize reserved input fields to zero; the kernel must not interpret them unless a later ABI version defines them. Output reserved fields must be initialized as specified by the individual interface.

Do not pass Rust `String`, `Vec`, references, slices, trait objects, or compiler-layout enums across the ABI.

## 6. Error convention — adopted

The raw syscall result/error encoding is adopted as follows:

- x86-64: result in RAX; RFLAGS.CF = 0 means success and RFLAGS.CF = 1 means failure.
- AArch64: result in X0; PSTATE.C = 0 means success and PSTATE.C = 1 means failure.
- On failure, the result register contains a positive value from the Wicofe errno namespace.
- On success, the result register contains the full operation result; negative signed success values are not reserved as errors.
- Wlibc maps raw errors to the relevant POSIX failure return and thread-local `errno`.

Canonical initial errno values and aliases are defined in [Errno Namespace 0.1](docs/ERRNO_NAMESPACE_0.1.md); register preservation is defined in [Syscall Register Preservation 0.1](docs/SYSCALL_REGISTER_PRESERVATION_0.1.md). The kernel must not write userspace TLS.

See [Wicofe syscall calling convention 0.1](docs/SYSCALL_CALLING_CONVENTION_PROPOSAL_0.1.md) and [technical review](docs/SYSCALL_CONVENTION_TECHNICAL_REVIEW_0.1.md).

## 7. Architecture policy

The initial target architectures are x86-64 and AArch64. The ABI defines `SYSCALL` and `SVC #0` as the entry mechanisms, with architecture-specific register assignments in the calling-convention document. Portable userspace headers must not expose architecture register details.

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

No syscall numbers or syscall identifier ranges are assigned by this calling-convention decision.
