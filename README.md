# AliceOS kernel

## About

The AliceOS kernel is a project to build a kernel according to AOS standards (more on this later).
Most of the code is C to make portability easier, other parts are assembly for either x86, x86_64, arm or mips (only the first currently used).
I'm not very experienced but I try to learn from other projects.

## Purpose

The project was started for multiple reasons.

First, I like to learn new things.
I like to study all kinds of things about software development, so why not learn about kernels?

Second, the Free Software community as a whole scares me.
I kinda want to stay away from all the chaos in this little island I built myself.

Third, I just want to make something that might at least give me a chance to be remembered in history.
I'm not asking too much, am I?

## Requirements

Building and linking requires the following:
 - clang (gcc currently does not work)
 - nasm
 - zsh
 - binutils
 - tar
 - xz

To test, you can install QEMU and use the included run_aliceos script.

## Development team

Mostly me, with help from other people.
For a list of contributors, check the CONTRIBUTORS file.

## Complaints

Might ignore them at first, but think things through later and possibly accept them.
