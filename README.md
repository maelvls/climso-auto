climso-auto
===========
Notes sur le format de ce fichier : .md permet de mettre en forme son texte avec des balises simples du type de "#" ou "_"

# Résumé
Dépot du projet de logiciel d'automatisation de l'ajustement de la monture équatoriale des appareil CLIMSO au Pic du Midi. Ce dépôt correspond à la partie développement du projet, et n'est pas en mode distribution : le code doit être associé aux outils autotool.

Caméra qui clignote toutes les secondes : une prise de vue est en cours.
## Notes
### Problème avec la connexion Arduino
### Problème avec la connexion SBIG
* "Permission denied" : il faut être root pour y accéder. La solution est d'ajouter la règle udev dans /usr/lib/udev/rules.d/51-mes-regles-climso.rules.

* "No error" : c'était une erreur qui apparaissait lorsque le modèle de la caméra n'était pas connu, mais là je ne sais pas d'où vient l'erreur

* "Camera Not Found" : 
 
