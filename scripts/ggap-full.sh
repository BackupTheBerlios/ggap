#!/bin/sh

script=$0

if [ -L $script ]; then
  script=`readlink $script`
fi

working_dir=`pwd`
ggap_dir=`dirname $script`
ggap_dir=`cd $ggap_dir && pwd`

setup_env()
{
  export MOO_DATA_DIRS=$ggap_dir/share/ggap
  export MOO_LIB_DIRS=$ggap_dir/lib/ggap
  export MOO_LOCALE_DIR=$ggap_dir/share/locale
}

setup_env_gtk()
{
  if [ -z "$FONTCONFIG_PATH" ]; then
    if [ -d /etc/fonts ]; then
      export FONTCONFIG_PATH=/etc/fonts
    else
      export FONTCONFIG_PATH=$ggap_dir/etc/fonts
    fi
  fi

  export GTK_IM_MODULE_FILE=$ggap_dir/etc/gtk-2.0/gtk.immodules
  export GTK_PATH=$ggap_dir/lib/gtk-2.0
  export LD_LIBRARY_PATH=$ggap_dir/lib:$LD_LIBRARY_PATH
  export XDG_DATA_DIRS=$ggap_dir/share:$XDG_DATA_DIRS
}

setup_env_python()
{
  export PYTHONHOME=$ggap_dir
}

setup_env_gap()
{
  export PATH=$ggap_dir/gap4r4/bin:$PATH
  export GAP_DIR=$ggap_dir/gap4r4
}

reinstall()
{
  rm -f .install-stamp

  printf "Creating gtk.immodules file... "
  rm -f etc/gtk-2.0/gtk.immodules
  sed -e "s@/GTK_ROOT/@$ggap_dir/@" etc/gtk-2.0/gtk.immodules.dist > etc/gtk-2.0/gtk.immodules
  printf "done\n"

  touch .install-stamp
}

install()
{
  if [ ! -f .install-stamp ]; then
    reinstall
  fi
}

run_ggap()
{
  install
  setup_env
  setup_env_gtk
  setup_env_python
  setup_env_gap
  cd $working_dir
  exec $ggap_dir/bin/ggap $*
}

cd $ggap_dir

if [ ! -z "$1" ]; then
  if [ "$1" = "--install" -o "$1" = "--reinstall" ]; then
    reinstall
    exit $?
  fi
fi

run_ggap $*
