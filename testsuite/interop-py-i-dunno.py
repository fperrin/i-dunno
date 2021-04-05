#!/usr/bin/env python3

import ipaddress
import os
from random import randint
from subprocess import run, PIPE
import sys
import unicodedata

try:
    import i_dunno
except:
    sys.stderr.write("Could not import i_dunno. Check installation")
    sys.exit(77)

def level2arg(level):
    return {"minimum": "-m",
            "satisfactory": "-s",
            "delightful": "-d"}[level]

def run_c2py(i_dunno_binary, level, addrs, verbose=False):
    # Form I-DUNNO using the C tool
    args = [i_dunno_binary, "-r0", level2arg(level)] + \
        [str(a) for a in addrs]
    if verbose:
        print(f"run_c2py: Running {args}")

    res = run(args, capture_output=True)

    nb_forms = 0

    # Now check we can loop back to original address
    i_dunno_forms = res.stdout.split(b'\0')
    for i, addr in enumerate(addrs):
        if verbose:
            print(f"{addr} ->", end=' ')

        i_dunno_form = i_dunno_forms[i]
        # Diddn't find a form for that address. Skip.
        if not i_dunno_form:
            if verbose:
                print(f"couldn't find valid form, trying next one...")
            continue

        if verbose:
            # Print the I-DUNNO form. First self-print all Unicode characters
            # that have a name, then print the codepoint and the name
            for cp in i_dunno_form.decode('utf-8'):
                if unicodedata.name(cp, None):
                    print(cp, sep='', end='')
                else:
                    print(f"\\u{ord(cp):04x}", sep='', end='')
            print()

            for cp in i_dunno_form.decode("utf-8"):
                print(f"    \\u{ord(cp):04x}: {unicodedata.name(cp, '')}")

        # Could find one, now check that the Python library can turn it back
        # into the original address
        roundtrip_addr = i_dunno.decode(i_dunno_form)
        assert roundtrip_addr == addr, \
            f"Decoding with python lib doesn't return original address: " \
            f"{addr} -> {i_dunno_form} -> {roundtrip_addr}"
        nb_forms += 1
    return nb_forms

def many_c2py(i_dunno_binary, level, addrtype, nb_tests, verbose=False):
    print(f"Running c2py {nb_tests} for {addrtype} AF at level {level}")
    while nb_tests > 0:
        addrs = []
        for i in range(nb_tests):
            if addrtype == "ip":
                maxaddr = 2 ** 32
            else:
                maxaddr = 2 ** 128
            addrs.append(ipaddress.ip_address(randint(1, maxaddr)))
        res = run_c2py(i_dunno_binary, level, addrs, verbose)
        nb_tests -= res

def c2py():
    i_dunno_binary = ""
    if "top_builddir" in os.environ:
        i_dunno_binary = f"{os.environ['top_builddir']}/src/i-dunno"
    # else running manually, make a best effort to find the binary
    elif os.path.exists("src/i-dunno"):
        i_dunno_binary = "src/i-dunno"
    elif os.path.exists("../src/i-dunno"):
        i_dunno_binary = "../src/i-dunno"

    many_c2py(i_dunno_binary, "delightful", "ip6", 1,
                 verbose=True)

    many_c2py(i_dunno_binary, "minimum", "ip", 10000)
    many_c2py(i_dunno_binary, "minimum", "ip6", 10000)
    many_c2py(i_dunno_binary, "satisfactory", "ip", 1000)
    many_c2py(i_dunno_binary, "satisfactory", "ip6", 1000)
    many_c2py(i_dunno_binary, "delightful", "ip", 10)
    many_c2py(i_dunno_binary, "delightful", "ip6", 100)

def one_py2py(level, addr):
    i_dunno_form = i_dunno.encode(addr, level=level)
    roundtrip_addr = i_dunno.decode(i_dunno_form)
    assert addr == roundtrip_addr, \
        f"{addr} -> {i_dunno_form} -> {roundtrip_addr}"

def many_py2py(level, addrlimit, nb_tests):
    while nb_tests > 0:
        addr = ipaddress.ip_address(randint(1, addrlimit))
        try:
            one_py2py(level, addr)
            nb_tests -= 1
        except ValueError:
            pass

def py2py():
    many_py2py("minimum", 2 ** 32 - 1, 10)
    many_py2py("minimum", 2 ** 128 - 1, 100)

def main():
    c2py()
    #py2py()

if __name__ == "__main__":
    main()
