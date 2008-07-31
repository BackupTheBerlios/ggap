AC_DEFUN([MOO_AC_PCH],[
  MOO_ENABLE_PCH=false
  AC_ARG_ENABLE([pch],AC_HELP_STRING([--enable-pch],[enable precompiled headers (default = NO)]),[
    if test "x$enableval" = xyes; then
      MOO_ENABLE_PCH=true
    fi
  ])
  AM_CONDITIONAL(MOO_ENABLE_PCH, $MOO_ENABLE_PCH)
])
