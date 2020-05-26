#!/bin/sh -x
exec clang `cat conf-cc` -c "$@"
