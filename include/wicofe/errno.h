/* SPDX-License-Identifier: MPL-2.0
 * Wicofe errno constants, ABI 0.1.
 * Canonical source: Wicofe-ABI/docs/ERRNO_NAMESPACE_0.1.md
 */
#ifndef WICOFE_ERRNO_H
#define WICOFE_ERRNO_H

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define EBUSY 15
#define EEXIST 16
#define EXDEV 17
#define ENODEV 18
#define ENOTDIR 19
#define EISDIR 20
#define EINVAL 21
#define ENFILE 22
#define EMFILE 23
#define ENOTTY 24
#define EFBIG 25
#define ENOSPC 26
#define ESPIPE 27
#define EROFS 28
#define EMLINK 29
#define EPIPE 30
#define EDOM 31
#define ERANGE 32
#define EDEADLK 33
#define ENAMETOOLONG 34
#define ENOLCK 35
#define ENOSYS 36
#define ENOTEMPTY 37
#define ELOOP 38
#define ENOMSG 39
#define EIDRM 40
#define EOVERFLOW 41
#define ECANCELED 42
#define EILSEQ 43
#define ENOTSOCK 44
#define EDESTADDRREQ 45
#define EMSGSIZE 46
#define EPROTOTYPE 47
#define ENOPROTOOPT 48
#define EPROTONOSUPPORT 49
#define ESOCKTNOSUPPORT 50
#define EOPNOTSUPP 51
#define EPFNOSUPPORT 52
#define EAFNOSUPPORT 53
#define EADDRINUSE 54
#define EADDRNOTAVAIL 55
#define ENETDOWN 56
#define ENETUNREACH 57
#define ENETRESET 58
#define ECONNABORTED 59
#define ECONNRESET 60
#define ENOBUFS 61
#define EISCONN 62
#define ENOTCONN 63
#define ETIMEDOUT 64
#define ECONNREFUSED 65
#define EHOSTUNREACH 66
#define EALREADY 67
#define EINPROGRESS 68
#define ESTALE 69
#define EDQUOT 70

#define EWOULDBLOCK EAGAIN
#define ENOTSUP EOPNOTSUPP

#endif
