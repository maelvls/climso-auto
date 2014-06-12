# configure.ac pour autoconf par Mael Valais - 2014
# Fichier utilisé pour la génération autoconf

AC_PREREQ([2.69])
AC_INIT([climso-auto], [1.0], [mael.valais@irap.omp.eu])


AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config])

# Les fichiers où sont stockées mes macros .m4 :
m4_include([m4/check_framework.m4])
m4_include([m4/autotroll.m4])
# Je devrais utiliser AC_CONFIG_MACRO_DIR() mais
# ça ne marche pas (car pas libtoolize?)

AM_INIT_AUTOMAKE([foreign subdir-objects])
# foreign pour ne pas creer INSTALL, README, NEWS...
#   subdir-objects pour ne pas faire du Makefile.am recursif,
#   du coup plus besoin de src/Makefile.am

# Vérifie si la librairie Qt est présente
AT_WITH_QT


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for GNU/Linux libraries or Mac OS frameworks 
AC_CHECK_LIB([m], [main],[],[exit])
AC_CHECK_LIB([tiff], [main],[],[
	AC_MSG_RESULT([--> La librairie -ltiff n'a pas ete trouvee])
	exit
])

AC_CHECK_LIB([sbigudrv], [main], [], [ 
	# La lib n'existe pas : on cherche un framework
	MY_CHECK_FRAMEWORK([sbigudrv])
	AS_IF([test "$my_cv_framework_sbigudrv" = "yes"],[
		# Le framework existe
		AC_MSG_RESULT([--> Le framework sbigudrv a bien ete trouve !])
		],[ # Le framework n'existe pas : erreur
		AC_MSG_ERROR([--> La librairie -lsbigudrv ou le framework sbigudrv est introuvable])
		exit
	])
])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h termios.h unistd.h tiffio.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([floor getcwd isascii memset sqrt strchr strstr])



AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT
AC_MSG_RESULT([-> LIBS : $LIBS /// $QT_LIBS /// $QTDIR])

