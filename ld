#!/bin/sh -x
exec clang `cat conf-ld` "$@"
