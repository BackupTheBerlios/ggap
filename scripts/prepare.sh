#!/bin/sh

if [ $1 ]; then
  cd $1
fi 

chmod +w lib/libpython*

rm -fr include man var
rm -fr lib/glib-2.0 lib/gtk-2.0/include lib/pango lib/pkgconfig lib/pygtk lib/xml2Conf.sh
rm -fr share/X11 share/aclocal share/doc share/glib-2.0 share/gtk-2.0 share/gtk-doc share/locale \
       share/man share/pixmaps share/pygobject share/pygtk share/xml share/gtk-engines share/applications

for d in bsddb/test config distutils email/test idlelib lib-dynload/_tkinter.so lib-old lib-tk test; do
  echo rm -fr lib/python2.4/$d
  rm -fr lib/python2.4/$d
done

for f in bin/*; do
  if test $f != "bin/ggap" -a $f != "bin/python"; then
    echo rm $f
    rm $f
  fi
done

find . -name "*.la" -print -delete
find . -name "*.so*" -print -exec 'strip' '--strip-unneeded' '{}' ';'
find lib/python2.4 -name "*.py" -print -delete
find lib/python2.4 -name "*.pyo" -print -delete

if [ -f etc/gtk-2.0/gtk.immodules ]; then
  sed -e "s@/.*/lib/gtk-2.0/@/GTK_ROOT/lib/gtk-2.0/@" \
      -e "s@/.*/share/locale@/GTK_ROOT/share/locale@" etc/gtk-2.0/gtk.immodules > etc/gtk-2.0/gtk.immodules.dist
  rm etc/gtk-2.0/gtk.immodules
fi

strip --strip-unneeded bin/*
