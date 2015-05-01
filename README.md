climso-auto
===========
[![Build Status](https://travis-ci.org/maelvalais/climso-auto.svg?branch=master)](https://travis-ci.org/maelvalais/climso-auto)
Dépot du projet de logiciel d'automatisation de l'ajustement de la monture équatoriale des appareil CLIMSO au Pic du Midi. Ce dépôt correspond à la partie développement du projet, et n'est pas en mode distribution : le code doit être associé aux outils autotool (autoconf 2.61 minimum) et donc il sera nécessaire d'utiliser `autoreconf`. Voir section "pour compiler".

La __TODO__ list et les problèmes à régler sont sur la page [`Issues`](https://github.com/mael65/climso-auto/issues). Les informations pour compiler et réparer l'arduino ou la caméra sont dans le [`Wiki`](https://github.com/mael65/climso-auto/wiki).

# Le système climso-auto
[Site du système CLIMSO](http://www.climso.fr/index.php/fr/climso/le-projet) pour comprendre le contexte du projet vis à vis des équipements CLIMSO.

## Schéma du système climso-auto
![Schéma simplifié du système](https://dl.dropboxusercontent.com/u/41771140/Image-climso-auto/arch-materielle.jpg)

La lisaison entre la caméra SBIG et l'ordinateur se fait à travers une rallonge USB par UTP.

## Schéma de fonctionnement de l'algorithme de guidage
![Schéma de fonctionnement de l'algorithme de détection du soleil](https://dl.dropboxusercontent.com/u/41771140/Image-climso-auto/algo-guidage.jpg)

Le schéma ne présente pas l'interface utilisateur ni les intéractions possibles.
Il s'agissait du programme de test lors des premières phases du projet.

## Objectifs du projet
Les observateurs associés, par équipes de deux, viennent opérer la coupole des coronographes pendant une semaine. Dans l'état actuel, un suivi automatique à gain constant permet de suivre le déplacement du soleil. Mais celui-ci ne suffit souvent pas et un repositionnement plus précis doit être effectué avant la prise d'image, toutes les cinq minutes.

Rendre ce _repositionnement précis_ automatique permettra donc de simplifier les procédures des observateurs et garder un centrage constant (rendant ainsi les films post-produits moins chaotiques).

## Performances et limitations du système

* Au niveau des performances, le logiciel échantillonne tous les 1000 à 1200ms et envoie des commandes tous les 5 échantillons. Les commandes sont calculées par simple moyenne.
* La caméra SBIG STF-8300M ne semble pas être adaptée à la capture de plusieurs images par seconde. Cela réduit le taux d'échantillonnage à une image par seconde maximum.

# Pour compiler

Librairies externes requises :
* `libtiff`, installable par `yum install libtiff`
* `libsbigudrv` (Linux) ou `SBIGUDrv.framework` (Mac), qu'on peut trouver sur le site sbig.com

On distinguera la compilation utilisateur final de la préparation de compilation développeur.
Le développeur utilise les fichiers de configuration autotools (configure.ac, Makefile.am...) ainsi que les outils (automake, autoconf..) alors que l'utilisateur final n'utilisera que les fichiers générés par ces outils (./configure, Makefile.in etc).

### Du côté du développeur (pour préparer la compilation)
Pour la mise à jour des fichiers de configuration de conmpilation :
	
	autoreconf 

Ça lancera les commandes autoconf, automake, aclocal. Si une erreur du type `configure.ac:29: error: required file '' not found`, lancer la commande
	
	automake --add-missing
	
Pour rendre le projet "portable" et ne plus nécessiter la suite Autotools :

	make distcheck (pour savoir si le tarball sera valide)
	make dist (pour générer le tarball .tar.gz du projet)

Cela creera un zip contenant le projet avec seulement les fichiers vitaux à destination de l'utilisateur final.

__Fichiers utilisés pour la configuration de la compilation :__

* `Makefile.am` dans tous les endroits donnés par `AC_CONFIG_FILES(Makefile:Makefile.in)` (écrits à la main, **obligatoire**)
* `configure.ac` (écrit à la main, **obligatoire**)
* autotroll.mk (pour gérer Qt)
* `m4/autotroll.m4`, (pour gérer Qt, vient de http://repo.or.cz/w/autotroll.git)
* `m4/check_framework.m4` (pour vérifier que le framework sbigudrv est présent)
	
### Du côté de l'utilisateur final (et pour les tests développeur)
Commandes de compilation :
	
	./configure
	make

__Fichiers utilisés lors de la compilation :__

- `Makefile.in` (listé(s) dans l'option `AC_CONFIG_FILES` de configure.ac)
- `configure`
- `config.h.in` (généré par autoheader)
- `config/compile` (généré par automake, pour les compilateurs ne gérant pas -c -o)
- `config/missing`, (généré par automake)
- `config/install-sh` (généré par automake)
- `config/decomp`
- `config/config.guess`

### Fichiers générés automatiquement
Certains fichiers sont générés lors de la compilation ou de la configuration du projet sont inutiles dans certains cas ; ces fichiers sont temporaires.

__Lors de la tâche de configuration de la compilation :__

* `configure`
* `aclocal.m4` (généré par `aclocal`)
* `config.h.in` (généré par autoheader)

__Lors de la tâche de compilation de l'executable :__

* `config.h`
* `Makefile`
* `config.log`
* `config.status` (permet de debugger)
* `stamp-h1` (??)

# Pour comprendre le projet
Pour maintenir le projet, il est nécessaire de comprendre la structure du projet que j'ai choisi.

![Schéma de structure du projet](https://dl.dropboxusercontent.com/u/41771140/Image-climso-auto/structure-fichiers.png)

### Lire les fichiers très commentés
Ces fichiers sont très commentés, les lire permettra sans doute de comprendre la structure choisie :

* `Makefile.am` contenant les fichiers à compiler
* `configure.ac` permettant de comprendre le fonctionnement de `./configure`

### Les fichiers source contenus (dans src/)
 
* Les fichiers pour le traitement des images :
 * `src/convol.c` (écrit par Laurent Koechlin, pour la corrélation)
 * `src/fcts_LK3.c` (écrit par LK, utilisé par convol.c)
 * `src/interpol.c` (écrit par LK, pour l'interpolation)
 * `src/diametre_soleil.c` (écrit par moi, permet de calculer le diamètre apparent du soleil en radians)
 * `src/image.cpp` (écrit par moi, utilisant les fichiers précédents)
 * `src/exceptions.cpp` (écrit par moi, gère les exceptions de la classe Image – amené à disparaitre car je pense que c'est inutile)


* Les fichiers pour l'Arduino :
 * `src/arduino.cpp` (écrit par moi, pour envoyer des commandes à l'arduino)


* Les fichiers pour la caméra SBIG :
 * `src/csbigcam.cpp` (fourni par le fabriquant)
 * `src/csbigimg.cpp` (fourni par le constructeur)

 
* Les fichiers gérant à proprement-dit l'interface
 * `src/main_qt_gui.cpp` (écrit par moi, permet de lancer l'application)
 * `src/fenetreprincipale.cpp` (écrit par moi, gère l'affichage des éléments et lance les deux autres processus légers (threads)
 * `src/widgetimage.cpp` (écrit par moi, gère l'affichage de l'image du soleil dans l'application)
 * 	`src/fenetreprincipale.ui` (est géré par QtDesigner et permet de dessiner la fenêtre de l'application – est transformé en `fenetreprincipale_ui.h` par _uic_. **Pour modifier l'interface, il faut ouvrir fenetreprincipale.ui avec QtDesigner ou QtCreator**)

* Les fichiers contrôlant l'interface (avec l'algorithme de guidage, la corrélation...)
 * `src/capture.cpp` (écrit par moi, est placé dans un thread différent, gère la capture de l'image SBIG, la corrélation et le diamètre du soleil. Envoie les résultats à guidage.cpp)
 * `src/guidage.cpp` (écrit par moi, gère la position de consigne, les envois des commandes à l'Arduino et l'affichage de l'image et des indicateurs gris/jaune/vert/rouge permettant de repérer le soleil)

* Les fichiers "à côté" :
 * les fichiers `*.moc.cpp` sont générés par _moc_ et permettent, pour toutes les classes implémentant un Q_OBJECT, de produire du méta-code.


# Notes d'utilisation de climso-auto
### Stockage des paramètres du logiciel
Grâce à QSettings, les parametres sont sauvés dans `~/.config/irap/climso-auto.conf` sous Linux, ou dans `~/Library/Preferences/com.irap.climso-auto.plist` sous MacOSX.

### Problème avec Arduino
* Impossible d'initialiser la connexion car le fichier spécial "/dev/usbACM0" (par exemple) n'appartient pas à l'utilisateur en cours. Pour réparer ça, il faut ajouter l'utilisateur en cours dans le groupe "dialup".

###Problèmes avec la caméra SBIG

* Si la caméra clignote toutes les secondes : une prise de vue est en cours. Si le logiciel est quitté d'une façon inappropriée, la caméra peut ne pas avoir été déconnectée correctement, et celle-ci peut s'être bloquée en mode prise de vue.

* `Permission denied` : il faut être root pour y accéder. La solution est d'ajouter la règle udev dans `/usr/lib/udev/rules.d/51-mes-regles-climso.rules`.

* `No error` : c'était une erreur qui apparaissait lorsque le modèle de la caméra n'était pas connu, mais là je ne sais pas d'où vient l'erreur. Il faut redémarrer la caméra.

* `Camera Not Found` : la caméra est sans doute déjà utilisée quelque part, à moins qu'elle ne soit pas connectée. Vérifiez qu'elle est bien établie en tant que périphérique USB par la commande `lsusb`. Si cela ne fonctionne pas, redémarrer la caméra.
 
# Explications
## Pourquoi ./configure, make...
Lorsqu'on programme en C/C++ et que l'objectif est de rendre le projet (c'est à dire l'ensemble des sources) portable, plusieurs options existent.

* __Le makefile__ :
L'option du makefile est la plus directe : on décrit comment on souhaite que la compilation se fasse et on appelle l'outil make. Mais cela manque de portabilité vis à vis des dépendances (librairies internes ou externes au système...). J'ai passé plus d'une semaine à approfondir ce langage de compilation, mais je pense qu'avec du recul, j'aurais dû simplement y passer 1 ou 2 jours et passer directement aux toolchains.

* __Les toolchains__ :
Ils permettent de construire automatiquement le makefile. Autotools vérifie d'abord les dépendances puis construit le makefile. Il existe aussi Qmake (lié à Qt) et Cmake (ou encore Imake).

C'est donc le toolchain _autoconf_ (automake, autoconf, aclocal...) que j'ai choisi. Il permet de vérifier si les librairies sont présentes de façon (assez) simple.

Mais bon, autoconf n'est pas facile à apprendre, donc un autre toolchain serait aussi bien...

Dans tous les cas, je ne voulais pas rester dans un IDE (type Xcode ou KDevelop) qui est très dépendant des versions. Il est assez simple de trouver la bonne version d'autoconf qui va bien (la 2.69 dans notre cas).

## Autoconf et Qt
Pour passer le projet Qmake que j'avais fait sous Qcreator, j'ai trouvé l'outil __Autotroll__. Il fallait aussi que le fichier interface (fenetreprincipale.ui) soit compilé par _uic_.

###Problème de sed dans ./configure avec autotroll.m4
J'ai remarqué des erreurs de parsing du fichier Makefile généré par Qmake pour trouver les flags CFLAGS etc. J'ai corrigé cela dans autotroll.m4.

### Problème avec la génération .ui.h :


Lorsque le mainwindow.ui.h était généré, le `#define MAINWINDOW_H` était le même que celui présent dans `mainwindow.h`. D'ou une erreur. 
Je suspecte **uic** de prendre toute la chaine avant le premier point : du coup `mainwindow.ui.h` devient `MAINWINDOW_H`.
J'ai donc décidé de passer, comme dans QCreator, les noms de fichiers générés en `mainwindow_ui.h` au lieu de `mainwindow.ui.h`. 

Dans `autotroll.mk` : 

	.ui.ui.h:
		$(UIC) $< -o $@
devient :

	%_ui.h: %.ui
		$(UIC) $< -o $@

### Utiliser la librairie Qt-3.3 au lieu de Qt4 (ou l'inverse)
Dans mon cas, j'avais installé plusieurs librairies Qt : `/usr/lib/qt4` et `/usr/lib/qt-3.3`. Le choix de la bonne librairie vient d'abord de ce qui est mis dans le PATH :

	echo $PATH
	/usr/lib/qt4/bin:/usr/lib/qt4/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/home/admin/.local/bin:/home/admin/bin

Par défaut, au démarrage de la machine, tous les .sh du dossier `/etc/profile.d` sont exécutés. Ces scripts ajoutent au PATH un morceau, ou alors ajoutent des variables globales...
Dans notre cas, le fichier `qt.sh` (pour qt-3.3) est responsable de l'ajout dans la variable `$PATH`. J'ai modifié le nom en `qt.sh.disabled` et crée le fichier `qt4.sh` qui contient l'ajout du dossier de qt4 au `$PATH`.

Du coup, les commandes qmake, moc, rcc, uic... sont lancées depuis le répertoire ciblé dans le `$PATH`.

Pour switcher entre qt-3.3 et qt4, il suffit donc de modifier le nom de `qt4.sh` en `qt4.sh.qqchose` et `qt.sh.disabled` en `qt.sh`.

# Eclipse, Xcode, Git et outils pour le projet

### Ouvrir ce projet _autotools_ (Makefile.am, configure.ac...) avec QtCreator
Il faut aller dans Aide > Plugins > et cocher AutotoolsProjectManager.

Ensuite on peut ouvrir le projet avec QtCreator en faisant Ouvrir fichier ou projet > on choisit le premier Makefile.am du projet et c'est bon.
Par contre seuls les fichiers donnés dans le Makefile.am seront affichés dans QtCreator, ce qui est une bonne chose pour ne pas se perdre !
Concernant le fichier .ui, je ne sais pas comment faire en sorte qu'il soit affiché dans QtCreator autrement qu'en l'ajoutant dans les `_SOURCES` de `Makefile.am`.

### Ouvrir le projet _autotools_ avec Xcode

Xcode n'a pas de fonctionnalité gérant directement le système _autotools_. En revanche, grâce à un système de cibles (targets), il est possible de développer sous Xcode.

On peut par exemple utiliser le binaire par défaut comme cible des sources, mais des problèmes de framework et librairies manquantes risquent d'empêcher d'utiliser pleinement l'IDE. 

### Ouvrir le projet _autotools_ avec Eclipse CDT
Eclipse est un IDE très élaboré sur lequel j'ai choisi de m'appuyer pour la plupart du développement de ce logiciel. De plus, un plugin _autotools_ existe. 

__Installer le plugin C/C++ Autotools support :__
Menu Help > Install new software > work with : -- All available sites-- > filtrer par "autotools" puis installez le.

__Importer le projet :__ Import > Existing code as Makefile project > GNU Autotools toolchain.

### Utiliser Git avec Eclipse CDT
Pour éviter de devoir taper un mot de passe, il faut d'abord avoir son propre certificat et l'avoir ajouté dans les préférences de son compte sur Github.
Il y a une différence lorsqu'on accède en https et en ssh.
En ssh, il suffit (si on a bien suivi https://help.github.com/articles/generating-ssh-keys) de changer l'url remote : https://help.github.com/articles/changing-a-remote-s-url

### Passer en mode debug
./configure CPPFLAGS="-DDEBUG=1"
