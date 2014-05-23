climso-auto
===========


# Résumé
Logiciel d'automatisation de l'ajustement de la monture équatoria'le des appareil CLIMSO au Pic du Midi

# Structure du projet

La structure n'est pas évidente : du code déjà existant est mélangé à du nouveau code. Ainsi, nous avons les sources existantes :

* 
* azdazd
* 

# Contenu

## Concernant la compilation
On utilise les autotools : autoconf, automake...



* ACLOCAL : Generate 'aclocal.m4' by scanning 'configure.ac' or 'configure.in'
* AUTOCONF : Generate a configuration script from configure.ac or configure.in.  Output is sent  to 'configure'.
* AUTOMAKE : Generate Makefile.in for configure from Makefile.am.
* AUTOHEADER : Create a template  file  of C '#define' statements for 'configure' to use.  To this end, scan 'configure.ac' if present, or else 'configure.in
* ./configure → prend Makefile.in pour fabriquer Makefile