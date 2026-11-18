/* SPDX-License-Identifier: MPL-2.0
 * Compile-time checks for Wicofe errno ABI 0.1.
 */
#include <wicofe/errno.h>

_Static_assert(EPERM == 1, "EPERM ABI value");
_Static_assert(ENOENT == 2, "ENOENT ABI value");
_Static_assert(EINTR == 4, "EINTR ABI value");
_Static_assert(EAGAIN == 11, "EAGAIN ABI value");
_Static_assert(EWOULDBLOCK == EAGAIN, "EWOULDBLOCK alias");
_Static_assert(EOPNOTSUPP == 51, "EOPNOTSUPP ABI value");
_Static_assert(ENOTSUP == EOPNOTSUPP, "ENOTSUP alias");
_Static_assert(EDQUOT == 70, "EDQUOT ABI value");

int wicofe_errno_contract_test(void)
{
    return 0;
}
