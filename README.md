climso-auto
===========
Notes sur le format de ce fichier : .md permet de mettre en forme son texte avec des balises simples du type de "#" ou "_"

# Résumé
Logiciel d'automatisation de l'ajustement de la monture équatoriale des appareil CLIMSO au Pic du Midi

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

## Notes
### Problème avec la connexion Arduino
### Problème avec la connexion SBIG
* "Permission denied" : il faut être root pour y accéder. La solution est d'ajouter la règle udev dans /usr/lib/udev/rules.d/51-mes-regles-climso.rules.

* "No error" : c'était une erreur qui apparaissait lorsque le modèle de la caméra n'était pas connu, mais là je ne sais pas d'où vient l'erreur

* "Camera Not Found" : 
 
