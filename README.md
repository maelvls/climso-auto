climso-auto
===========
Dépot du projet de logiciel d'automatisation de l'ajustement de la monture équatoriale des appareil CLIMSO au Pic du Midi. Ce dépôt correspond à la partie développement du projet, et n'est pas en mode distribution : le code doit être associé aux outils autotool.

# Le système climso-auto
Schéma de fonctionnement de l'algorithme de guidage :
![https://dl.dropboxusercontent.com/u/41771140/Image-climso-auto/algo-guidage.png](image)

Schéma du système :
![https://dl.dropboxusercontent.com/u/41771140/Image-climso-auto/arch-materielle.png](image)

# Pour compiler

On distinguera la compilation utilisateur final de la préparation de compilation développeur.
Le développeur utilise les fichiers de configuration autotools (configure.ac, Makefile.am...) ainsi que les outils (automake, autoconf..) alors que l'utilisateur final n'utilisera que les fichiers générés par ces outils (./configure, Makefile.in etc).

### Du côté du développeur (pour préparer la compilation)
Pour la mise à jour des fichiers de configuration de conmpilation :
	
	autoreconf 

Ça lancera les commandes autoconf, automake, aclocal. 

Si une erreur du type :
	
	configure.ac:29: error: required file 'config/	config.guess' not found

lancez la commande
	
	automake --add-missing
	
Pour rendre le projet "portable" et ne plus nécessiter la suite Autotools :

	make distcheck (pour savoir si le tarball sera valide)
	make dist (pour générer le tarball .tar.gz du projet)

Cela creera un zip contenant le projet avec seulement les fichiers vitaux à destination de l'utilisateur final.

__Fichiers utilisés pour la configuration de la compilation :__

* `Makefile.am` dans tous les sous-dossiers (écrits à la main)
* `configure.ac` (écrit à la main)
* autotroll.mk (pour gérer Qt)
* `m4/autotroll.m4`, (pour gérer Qt)
* `m4/check_framework.m4` (pour vérifier que le framework sbigudrv est présent)
* `config.h.in` éventuellement (généré par Autoheader)
	
### Du côté de l'utilisateur final (et pour les tests développeur)
Commandes de compilation :
	
	./configure
	make

Fichiers utilisés par cette compilation :

- `Makefile.in` (listé(s) dans l'option `AC_CONFIG_FILES` de configure.ac)
- `configure`
- `config/compile` (généré par automake, pour les compilateurs ne gérant pas -c -o)
- `config/missing`, (généré par automake)
- `config/install-sh` (généré par automake)
- `config/decomp`
- `config/config.guess`

### Fichiers générés automatiquement
Certains fichiers sont générés lors de la compilation ou de la configuration du projet sont inutiles dans certains cas ; ces fichiers sont temporaires.

Lors de la tâche de compilation de l'executable :

* `config.h`
* `Makefile`
* `config.log`
* `config.status` (permet de debugger)
* `stamp-h1` (??)

Lors de la tâche de configuration de la compilation :

* `configure`
* `aclocal.m4` (généré par `aclocal`)


# Pour comprendre le projet
Pour maintenir le projet, il est nécessaire de comprendre la structure du projet que j'ai choisi.

### Lire les fichiers très commentés
Ces fichiers sont très commentés, les lire permettra sans doute de comprendre la structure choisie :

* `Makefile.am` contenant les fichiers à compiler
* `configure.ac` permettant de comprendre le fonctionnement de `./configure`

### Les fichiers contenus dans src/
Il y a trois groupes de fichiers :
 
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
 * `src/fenetreprincipale.moc.cpp` (généré automatiquement par _moc_ à partir de fenetreprincipale.h)
 * `src/widgetimage.cpp` (écrit par moi, gère l'affichage de l'image du soleil dans l'application)
 * `src/widgetimage.moc.cpp` (généré par _moc_ à partir de widgetimage.h)


* Les fichiers contrôlant l'interface (avec l'algorithme de guidage, la corrélation...)
 * `src/capture.cpp` (écrit par moi, est placé dans un thread différent, gère la capture de l'image SBIG, la corrélation et le diamètre du soleil. Envoie les résultats à guidage.cpp)
 * `src/capture.moc.cpp` (généré par _moc_)
 * `src/guidage.cpp` (écrit par moi, gère la position de consigne, les envois des commandes à l'Arduino et l'affichage de l'image et des indicateurs gris/jaune/vert/rouge permettant de repérer le soleil)
 * `src/guidage.moc.cpp` (généré par _moc_


* Les fichiers "à côté"
 * 	`src/fenetreprincipale.ui` (généré par QtDesigner et permet de dessiner la fenêtre de l'application – est transformé en `fenetreprincipale_ui.h` par _uic_)

# Notes
### Utiliser Git avec Eclipse CDT
Pour éviter de devoir taper un mot de passe, il faut d'abord avoir son propre certificat et l'avoir ajouté dans les préférences de son compte sur Github.
Il y a une différence lorsqu'on accède en https et en ssh.
En ssh, il suffit (si on a bien suivi https://help.github.com/articles/generating-ssh-keys) de changer l'url remote : https://help.github.com/articles/changing-a-remote-s-url
### Problème avec Arduino
* Impossible d'initialiser la connexion car le fichier spécial "/dev/usbACM0" (par exemple) n'appartient pas à l'utilisateur en cours. Pour réparer ça, il faut ajouter l'utilisateur en cours dans le groupe "dialup"


### Problème avec SBIG
* Caméra qui clignote toutes les secondes : une prise de vue est en cours.

###Problèmes de connexion
* "Permission denied" : il faut être root pour y accéder. La solution est d'ajouter la règle udev dans /usr/lib/udev/rules.d/51-mes-regles-climso.rules.

* "No error" : c'était une erreur qui apparaissait lorsque le modèle de la caméra n'était pas connu, mais là je ne sais pas d'où vient l'erreur

* "Camera Not Found" : 
 
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

## Qt
### Problème avec -qt-lm non trouvé (qt-3.3)
Ce problème provient des -L/librairies manquants. J'ai vu que la variable `$QT_LIBS` est celle contenant tous les -l et -L. Or, la voici sur le Fedora :
	
	QT_LIBS=-L$(QT_QTDIR)/lib -lqt-mt -lXext -lX11 -lm
	QT_QTDIR=
On voit que `$(QT_QTDIR)` n'a pas été expended. Sur le mac, `$QT_LIBS` ne donne pas de `-L$(QT_QTDIR)`.
J'ai vu que `$(QTDIR)` contient par contre le bon chemin, ce qui permettrait à `$QT_LIBS` de se compléter correctement.
	
	QT_LIBS=/usr/lib/qt-3.3

ce qui correspond bien au chemin manquant.

On sait aussi que `autotroll.m4` prend toutes les variables de type `$BLABLA` et y préfixe en `$QT_BLABLA`.

Donc je pense qu'il y a un problème par là.

* `autotroll.m4:348` Le filtre SED `$qt_sed_filter` met `QT_` devant tous les noms de variable. 

J'ai aussi découvert que **qmake utilisé était celui de qt-3.3** (en faisant which qmake). 

J'ai donc renommé `/etc/profile.d/qt.sh` en `/etc/profile.d/qt.sh.disabled`. Ensuite j'ai ajouté `/etc/profile.d/qt4.sh` avec dedans :
	
	QT4DIR=/usr/lib/qt4
	export PATH=/usr/lib/qt4/bin:$PATH
	export PKG_CONFIG_PATH=/usr/lib/qt4/lib/pkgconfig:$PKG_CONFIG_PATH
	export QT4DIR

**Du coup, qmake est bien celui du qt4. Et cette fois ça marche parfaitement.**

 Manque des includes dans Eclipse CDT (dans la liste "Includes" de l'explorateur) mais ce problème s'est résolu tout seul (comment ? CDT a trouvé les chemins des includes tout seul ?? Non...)

### Ouvrir ce projet "autotools" (Makefile.am, configure.ac...) avec QTCreator
Il faut aller dans Aide > Plugins > et cocher AutotoolsProjectManager.

Ensuite on peut ouvrir le projet avec QtCreator en faisant Ouvrir fichier ou projet > on choisit le premier Makefile.am du projet et c'est bon.
Par contre seuls les fichiers donnés dans le Makefile.am seront affichés dans QtCreator, ce qui est une bonne chose pour ne pas se perdre !
Concernant le fichier .ui, je ne sais pas comment faire en sorte qu'il soit affiché dans QtCreator autrement qu'en l'ajoutant dans les `_SOURCES` de `Makefile.am`.

