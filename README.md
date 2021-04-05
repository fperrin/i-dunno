An implementation of I-DUNNO, or _Internationalized Deliberately
Unreadable Network NOtation_, addresses, as specified in
[RFC 8771](https://tools.ietf.org/html/rfc8771):

> Domain Names were designed for humans, IP addresses were not.  But
> more than 30 years after the introduction of the DNS, a minority of
> mankind persists in invading the realm of machine-to-machine
> communication by reading, writing, misspelling, memorizing,
> permuting, and confusing IP addresses.  This memo describes the
> Internationalized Deliberately Unreadable Network NOtation
> ("I-DUNNO"), a notation designed to replace current textual
> representations of IP addresses with something that is not only more
> concise but will also discourage this small, but obviously important,
> subset of human activity.

# i-dunno(1)

## SYNOPSIS

`i-dunno` `[-sd]` `ADDRESS`

## DESCRIPTION

Prints an I-DUNNO representation of ADDRESS (IPv4 or IPv6).

**-s**
: generate Satisfactory level of confusion

**-d**
: generate Delightful level of confusion

**-p**
: do not pad the source when forming address

**-r**
: randomize list of codepoints considered when forming I-DUNNO

The Mininum level of confusion is always enabled.

The address is printed as a UTF-8 string followed with a newline on stdout.
**`i-dunno`** doesn't care if the current locale is not UTF-8.

Exits with a non-zero status if the requested level of confusion cannot be
reached. Sadly there are many addresses where a form of the requested
confusion level can't be found, especially at the delightful level.

# i_dunno_form(3)

## SYNOPSIS

```c
#include <i-dunno.h>

const char *i_dunno_form(int af, const void *src,
                         char *dst, socklen_t size,
                         int flags);
```

## DESCRIPTION

This function forms an I-DUNNO address.

**af**
: one of *`AF_INET`* or *`AF_INET6`*.

**src**
: points to a *`struct in_addr`* or *`struct in6_addr`*, which is
  formed into an I-DUNNO representation.

**dst**
: points to a buffer of size *`size`* where the resulting I-DUNNO
  representation is stored. That buffer should be at least
  *`I_DUNNO_ADDRSTRLEN`* long. The representation is stored in UTF-8.

**flags**
: controls the forming operation. It is the bitwise-or of a required
  confusion level, one of the following:

  - *`I_DUNNO_MINIMUM_CONFUSION`*,

  - *`I_DUNNO_SATISFACTORY_CONFUSION`*,

  - *`I_DUNNO_DELIGHTFUL_CONFUSION`*;

  and zero or more of the following options:

  - *`I_DUNNO_NO_PADDING`*: do not pad the bitstream when allocating
    codepoints forming the I-DUNNO representation.

## RETURN VALUE

**`i_dunno_form`** returns *`buf`* if it succeeds, or *`NULL`* otherwise. In
case of failure, *`errno`* might contain a clue of what went wrong, but most
likely won't. *`dst`* is bound to be modified even on failure.

# i_dunno_deform(3)

## SYNOPSIS

```c
#include <i-dunno.h>

int i_dunno_deform(int af, const char *src, void *dst);
```

## DESCRIPTION

This function deforms an I-DUNNO address.

**af**
: one of *`AF_INET`* or *`AF_INET6`*.

**src**
: points to a I-DUNNO address.

**dst**
: points to a *`struct in_addr`* or *`struct in6_addr`* where the
  I-DUNNO representation is deformed.


## RETURN VALUE

`1` on success, `0` on failure.
