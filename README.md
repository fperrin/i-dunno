This library allows to form and deform Internationalized Deliberately
Unreadable Network NOtation, or I-DUNNO, addresses, as specified in
[RFC 8771](https://tools.ietf.org/html/rfc8771).

# CLI tool

## SYNOPSIS

`i-dunno` `[-sd]` `ADDRESS`

## DESCRIPTION


Forms an I-DUNNO representation of ADDRESS (IPv4 or IPv6).

- *-s* generates Satisfactory level of confusion

- *-d* generates Delightful level of confusion

The Mininum level of confusion is always enabled.

The address is printed as a UTF-8 string followed with a newline on
stdout. ~i-dunno~ doesn't care if the current locale is not UTF-8.

Exits with a non-zero status if a satisfactory level of confusion
cannot be reached. Sadly there are many addresses where a form of the
requested confusion level can't be found, especially at the delightful
level.

# API Usage

# `i_dunno_form`

### SYNOPSIS

    #include <i-dunno.h>

    const char *i_dunno_form(int af, const void *src,
                             char *dst, socklen_t size,
                             int flags);

### DESCRIPTION

This function forms an I-DUNNO address.

- *af* is one of `AF_INET`, `AF_INET6`.

- *src* points to a `struct in_addr` or `struct in6_addr`, which is
  formed into an I-DUNNO representation.

- *dst* points to a buffer of size `size` where the resulting
  I-DUNNO representation is stored. That buffer should be at least
  `I_DUNNO_ADDRSTRLEN` long.

- *flags* specifies the required complexity of the final address. The
  following values can be specified:

  - `I_DUNNO_MINIMUM_CONFUSION`

  - `I_DUNNO_SATISFACTORY_CONFUSION`

  - `I_DUNNO_DELIGHTFUL_CONFUSION`

### RETURN VALUE

`i_dunno_form` returns `buf` if it succeeds, or `NULL` otherwise. In
case of failure, `errno` might contain a clue of what went wrong, but
most likely won't. `dst` is bound to be modified even on failure.

## ~i_dunno_deform~

### SYNOPSIS

    #include <i-dunno.h>

    int i_dunno_deform(int af, const char *src, void *dst);

### DESCRIPTION

This function deforms an I-DUNNO address.

- *af* is one of `AF_INET`, `AF_INET6`.

- *src* points to a I-DUNNO address.

- *dst* points to a `struct in_addr` or `struct in6_addr` where the
  I-DUNNO representation is deformed.


### RETURN VALUE

`1` on success, `0` on failure.
