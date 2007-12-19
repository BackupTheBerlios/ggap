#!/bin/sh

script=$0

if test -L $script; then
  script=`readlink $script`
fi

ggap_dir=`dirname $script`
ggap_dir=`cd $ggap_dir && pwd`

export MOO_DATA_DIRS=$ggap_dir/share/ggap
export MOO_LIB_DIRS=$ggap_dir/lib/ggap
export MOO_LOCALE_DIR=$ggap_dir/share/locale

export PATH=$ggap_dir/bin:$PATH

export LD_LIBRARY_PATH=$ggap_dir/lib:$LD_LIBRARY_PATH

exec $ggap_dir/bin/ggap $*