# Syscall Calling Convention Technical Review 0.1

- **Status:** Architecture decision adopted; implementation validation pending.
- **Adopted convention:** [Wicofe Syscall Calling Convention 0.1](SYSCALL_CALLING_CONVENTION_PROPOSAL_0.1.md).
- **Architectures:** x86-64 and AArch64.
- **Decision:** Use RFLAGS.CF on x86-64 and PSTATE.C on AArch64 as the syscall success/error discriminator.

## 1. Decision

The Wicofe syscall ABI adopts this result encoding on both initial architectures:

- Success: operation result in the primary result register; carry clear.
- Failure: positive Wicofe errno in the primary result register; carry set.
- The carry bit is an out-of-band discriminator, so a successful negative signed result does not collide with an error.

This is a Wicofe ABI decision using architectural processor state. It is not a convention mandated by Intel, AMD, or Arm, and it does not provide Linux syscall ABI compatibility.

Adoption freezes the direction of the raw result/error encoding. It does not declare the complete userspace ABI stable and does not waive the implementation and runtime validation requirements below.

## 2. x86-64 findings and kernel obligations

SYSCALL saves the post-instruction RIP in RCX and RFLAGS in R11, then applies the OS-configured IA32_FMASK mask to live kernel RFLAGS. SYSRETQ restores user RIP from RCX and user RFLAGS from R11. Therefore CF is the result discriminator if the kernel deliberately writes the result CF into the saved return RFLAGS image.

Mandatory constraints:
1. Save the user return state before kernel instructions modify flags.
2. Set or clear CF in the saved user RFLAGS image after all flag-changing kernel work.
3. Validate user RIP and use a fully specified safe IRETQ fallback where SYSRETQ is invalid.
4. Document IA32_FMASK; it must not erase the saved user CF result.
5. Declare RCX and R11 clobbers in the userspace stub.
6. Capture CF as an output of the same compiler-visible assembly operation as SYSCALL.
7. Specify the user condition-code clobber/result contract; do not promise preservation of unrelated arithmetic flags without a separate mechanism.

## 3. AArch64 findings and kernel obligations

On exception entry to EL1, PSTATE is captured in SPSR_EL1. ERET restores PSTATE from SPSR_EL1, including condition flags. The kernel therefore returns X0 as the result and PSTATE.C as the error discriminator.

Mandatory constraints:
1. Retain the original user SPSR and distinguish it from the syscall result C bit.
2. Set or clear SPSR_EL1.C according to the syscall result immediately before ERET.
3. Capture C in the same assembly block as SVC before compiler output can change flags.
4. Treat X8 as clobbered and X0 as input/output; document the preservation contract for all other GPRs.
5. Specify signal-frame delivery/return behavior for the user condition state.

## 4. Compiler and assembly obligations

The syscall boundary must be a small architecture-specific assembly operation. It must:
- perform the trap and capture carry in one compiler-visible asm boundary;
- declare every register input, output, and clobber explicitly;
- include the appropriate memory clobber or equivalent wrapper contract because syscalls access memory through user pointers;
- prevent compiler motion across memory operations where syscall semantics require ordering;
- be validated under supported GCC and Clang target triples and optimization levels, including LTO if enabled.

## 5. Required runtime test matrix

| Test | x86-64 | AArch64 |
|---|---:|---:|
| Success returns zero with carry clear | Required | Required |
| Success returns positive value with carry clear | Required | Required |
| Success returns negative signed value with carry clear | Required | Required |
| Error returns positive errno with carry set | Required | Required |
| Documented GPR preservation | Required | Required |
| Carry survives interrupt/preemption path | Required | Required |
| Invalid user return address takes safe return path | Required | Architecture-specific equivalent |
| Signal delivery/return preserves specified context | Required | Required |
| GCC and Clang optimized stub tests | Required | Required |
| Kernel and userspace agree on errno constants | Required | Required |

Tests must run under QEMU for both architectures. Build-only compilation is not runtime flag verification.

## 6. Remaining gates before ABI stability

- Canonical errno namespace and generated public headers.
- Exact GPR preservation and user condition-code clobber contract.
- Signal-frame representation and restoration.
- Syscall identifier ranges and ABI version/capability discovery.
- Per-syscall blocking, interruption, restart, and cancellation semantics.
- Kernel and userspace QEMU runtime evidence for both architectures.

The architecture decision is adopted. These gates remain mandatory before claiming implementation completeness or ABI stability.
