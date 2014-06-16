# configure.ac pour autoconf par Mael Valais - 2014
# Fichier utilisé pour la génération autoconf

AC_PREREQ([2.69])
AC_INIT([climso-auto], [1.0], [mael.valais@irap.omp.eu])

# Vérifie que le ./configure agira bien sur les fichiers du projet ; simple test
AC_CONFIG_SRCDIR([src/image.cpp])

AC_CONFIG_HEADERS([config.h:config.h.in])
# Dossier contenant les scripts de configuration à utiliser par l'utilisateur
AC_CONFIG_AUX_DIR([config])

# Les fichiers où sont stockées mes macros .m4 :
m4_include([m4/check_framework.m4])
m4_include([m4/autotroll.m4])
# Je devrais utiliser AC_CONFIG_MACRO_DIR() mais
# ça ne marche pas (car pas libtoolize?)

# Initiation du script d'automake
# foreign pour ne pas obliger à avoir INSTALL, README, NEWS...
# subdir-objects permet de garder les .o dans leurs dossiers respectifs
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Vérifie si la librairie Qt est présente en utilisant qmake (grâce à autotroll.m4)
# et configure divers variables ($QT_LIBS etc) - voir dans autotroll.m4
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

# Fichiers Makefile à configurer ; ici, on en utilise qu'un seul mais en général, on
# en utilise pour chaque sous-dossier source (on aurait pu mettre src/Makefile).
# A chaque emplacement où on ajoute un Makefile, il faut avoir un Makefile.am.
AC_CONFIG_FILES(Makefile:Makefile.in) 
AC_OUTPUT
