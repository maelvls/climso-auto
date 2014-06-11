climso-auto
===========


# Résumé
Logiciel d'automatisation de l'ajustement de la monture équatoria'le des appareil CLIMSO au Pic du Midi

### Le source du micro-programme d'Arduino
https://gist.github.com/mael65/b2b2e67395e6c45ad814#file-arduino-ino

# Structure du projet

Pour le moment, il y a deux "branches" :
* master : projet à un état fonctionnel en ligne de commande,
* essai-qt : état de test pour l'interface Qt, mais aussi des améliorations dans les algorithmes et dans la classe Image et Arduino

Le mieux est de choisir la deuxième branche


# Contenu
## Concernant la compilation
On utilise les autotools : autoconf, automake...



* ACLOCAL : Generate 'aclocal.m4' by scanning 'configure.ac' or 'configure.in'
* AUTOCONF : Generate a configuration script from configure.ac or configure.in.  Output is sent  to 'configure'.
* AUTOMAKE : Generate Makefile.in for configure from Makefile.am.
* AUTOHEADER : Create a template  file  of C '#define' statements for 'configure' to use.  To this end, scan 'configure.ac' if present, or else 'configure.in
* ./configure → prend Makefile.in pour fabriquer Makefile

## Notes
### Problème avec la connexion SBIG
* "Permission denied" : il faut être root pour y accéder. La solution est d'ajouter la règle udev dans /usr/lib/udev/rules.d/51-mes-regles-climso.rules.

* "No error" : c'était une erreur qui apparaissait lorsque le modèle de la caméra n'était pas connu, mais là je ne sais pas d'où vient l'erreur

* "Camera Not Found" : certainement que la caméra est utilisée dans un autre programme
 
