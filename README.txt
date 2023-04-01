# A propos
Outlive The Blank est un projet universitaire réalisé à la première session de la maîtrise en Informatique Cheminement Jeu Vidéo à l’Université de Sherbrooke, à l’automne 2022.. Il s’agit d’un projet réalisé dans le cadre du cours d’Infographie pour le Jeu Vidéo et du cours de Mathématiques et Physique appliqués au Jeu Vidéo.
Le projet a été effectué sur environ 6 semaines, à temps partiel, en équipe de 5 étudiants : Arthur Lewis, Alistair Kenny, Romain Berthon, Alexandre Dubuc et moi-même, Titouan Bricout. Le projet était encadré par Mykel Leclerc Brisson.

Aucun moteur commercial n’a été utilisé, le jeu est réalisé directement à partir de DirectX 11 (et PhysX pour la partie physique).
Ma contribution principale au projet porte sur le pipeline de shaders, la gestion du transfert en mémoire vidéo, et surtout les lumières et les ombres (avec notamment l’utilisation de shadow mapping).


# Installation

Pour partir le projet, deziper, ouvrir la solution, choisir Debug ou Release, partir le projet

# Contrôles

## Déplacements au sol

* **"W"** pour avancer
* **"S"** pour reculer
* **"A"** pour tourner à gauche
* **"D"** tourner à droite
* **"Espace"** pour sauter

## Déplacements aérien

* **"W"** pour avancer
* **"S"** pour reculer
* **"A"** pour faire un léger déplacement vers la gauche
* **"D"** pour faire un léger déplacement vers la droite

## Contrôles généraux
* **"Clic gauche"** tir du canon pour une impulsion vers l'arrière
* **"Tab"** changer de caméra (1er personne et 3e personne)
* **"La souris vers la gauche et droite"** pour tourner à gauche et droite
* **"L"** test pour la lumière


# Options

## Menu principal

* Jouer
* Accéder aux options
* Quitter le jeu

## Menu in game
* Reprendre
* Recommencer
* Ouvrir les options
* Revenir au menu principal

## Options
Dans les options vous pouvez régler :
* L'audio
    * 25%
    * 50%
    * 75%
    * 100%
* La difficulté
    * Facile (testing / exploration)
    * Moyen (découverte)
    * Difficile (bon challenge)
    * Fury (quasiment impossible)
* La sensibilité
    * 25%
    * 50%
    * 75%
    * 100%

Le réglage de résolution n'est pas activé pour éviter des bugs d'affichages observés lors du développement.
