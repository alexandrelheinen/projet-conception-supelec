# projet-conception-supelec
Projet de Conception Supélec - livrable juin 2015
Binôme 66 : Alexandre Loeblein Heinen | Clyvian Ribeiro Borges

Ce projet a été développé dans le cadre du cursus d'ingénieur Supélec entre les mois d'avril et juin 2015. Il s'agit d'une interface entre MATLAB et Arduino qui permet le balayage des servos et la lectures des températures à partir du capteur optique MLX90620.

Pour les références et informations générales dur son développement, merci de regarder le rapport final `rapport.pdf` et pour sa mise en fonctionnement, le fichier `mode_demploi.pdf` vous fournira ce qu'il vous faut.

Le principal apport de ce projet c'est la bibliothèque *MLX90620.h* (`arduino/libraries/MLX90620`) qui fourit des fonctions pour la mise en fonctionnement du capteur MLX90620 à partir du microcontrôleur Arduino (dans le projet on a employé la version Uno). MLX90620.h utilise la bibliothèque [I2Cmaster] (https://github.com/DSSCircuits/I2C-Master-Library) qui n'est pas native de l'IDE Arduino et qui a été donc ajoutée aux fichiers du projet.

Si vous utilisez l'[IDE Arduino] (https://www.arduino.cc/en/Main/Software), il vous faudra copier le dossier `MLX90620` dans le dossier des blibliothèques d'Arduino, normalement trouvé dans `arduino_installation_path\libraries\` ou l'installer par le gestionnaire de bibliothèques de l'IDE.
