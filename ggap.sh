#!/bin/sh

if test ! -e ggap-bin/ggap.sh; then
  if test ! -e Makefile; then
    ./configure || exit 1
  fi
  make bin-dist || exit 1
fi

exec ggap-bin/ggap.sh $*
