# ABI Documentation Index

This directory contains documentation for the public Wicofe userspace/kernel ABI. Canonical syscall identifiers and user-visible layouts are owned by this repository.

## Canonical specification

- [Specification baseline](../SPECIFICATION_BASELINE.md)
- [Osynit ABI requirements 0.1](OSYNIT_ABI_REQUIREMENTS_0.1.md) — capability requirements without syscall assignments.
- [Osynit ABI requirements 0.1](OSYNIT_ABI_REQUIREMENTS_0.1.md) — required capability domains; assigns no syscall numbers or layouts.

## Scope

Document syscall numbers, architecture calling conventions, argument widths, pointer rules, structures, errors, process/thread interfaces, file descriptors, signals, time, memory mapping, compatibility, and generated bindings.

The kernel implements this contract in [Wicofe-Kernel](https://github.com/Waizusora/Wicofe-Kernel). C runtime adaptation belongs to [Wicofe-Libc](https://github.com/Waizusora/Wicofe-Libc); safe Rust userspace wrappers belong to [Wicofe-Lib](https://github.com/Waizusora/Wicofe-Lib).

## Stability

This ABI is a draft until individual interfaces have explicit versioning, error semantics, architecture coverage, layout tests, and compatibility policy. Never infer ABI stability from an implementation detail or Rust type layout.

## Related documentation

- [Central Wicofe documentation index](https://github.com/Waizusora/Wicofe-Docs/blob/main/INDEX.md)
- [Repository documentation map](https://github.com/Waizusora/Wicofe-Docs/blob/main/REPOSITORY_DOCUMENTATION_MAP.md)
