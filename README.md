# AliceOS kernel

## About

The AliceOS kernel is a project to build a kernel according to AOS standards (more on this later).
Most of the code is C to make portability easier, other parts are assembly for either x86, x86_64, arm or mips (only the first currently used).
I'm not very experienced but I try to learn from other projects.

## Purpose

The project was started for multiple reasons.

First, I like to learn new things. I like to study all kinds of things about software development, so why not learn about kernels?

Second, a mere dislike for anything GNU-related. I don't like software being used as some sort of weapon in some crazy revolution.
I'd give less of a damn about the "free software movement". I prefer to side with open source because heck, it's practical.

Third, I just want to make something that might at least give me a chance to be remembered in history. I'm not asking too much, am I?

## Requirements

Building and linking requires the following:
 - Clang (NCSA)
 - NASM (New BSD)
 - Zsh (MIT-like)
 - GNU binutils (GPLv2)
GNU binutils is required for linking. The AOS standard doesn't mention anything against using GNU software for linking (only if it's for "building").

To test, you can install QEMU and use the included run_aliceos script.

## Development team

Mostly me, with help from other people. For a list of contributors, check the CONTRIBUTORS file.

## Complaints

Redirected to /dev/null if they're not valuable, especially if they come from raging defenders of free software.
