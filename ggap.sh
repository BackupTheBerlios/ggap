#!/bin/sh

ggap_dir=`dirname $0`
ggap_dir=`cd $ggap_dir && pwd`

export MOO_DATA_DIRS=$ggap_dir/share/ggap
export MOO_LIB_DIRS=$ggap_dir/lib/ggap
export MOO_LOCALE_DIR=$ggap_dir/share/locale

export LD_LIBRARY_PATH=$ggap_dir/lib:$LD_LIBRARY_PATH

exec $ggap_dir/bin/ggap $*
