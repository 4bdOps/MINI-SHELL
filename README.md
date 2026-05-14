# MINI-SHELL


# PROJET : MINI-SHELL

Ce projet consiste en la conception et le développement d'un interpréteur de commandes simplifié. L'objectif est de reproduire le fonctionnement d'un shell Unix/Linux dans un environnement contrôlé, en mettant l'accent sur la gestion des processus et du système de fichiers.

---

## 1. PRESENTATION DU SUJET

Le Mini-Shell est un programme capable d'interagir avec un système d'exploitation via une interface textuelle. Il permet l'exécution de commandes de base et simule la logique interne d'un interpréteur de commandes réel.

---

## 2. FONCTIONNALITES

### | Fonctionnalités Minimales
* Parsing des commandes : Analyse syntaxique de la ligne d'entrée pour identifier les commandes et leurs arguments.
* Exécution des commandes internes : Gestion native des commandes suivantes :
  - `cd`     : Changement de répertoire courant.
  - `ls`     : Liste du contenu d'un répertoire.
  - `mkdir`  : Création de nouveaux répertoires.
* Historique : Sauvegarde et rappel des commandes précédemment exécutées.

### | Fonctionnalités Avancées
* Redirection et Pipelining : 
  - Redirection de sortie vers un fichier (`>`).
  - Enchaînement de commandes via des pipes (`|`).
* Scripts : Support pour l'exécution de fichiers contenant des listes de commandes.

---

## 3. TRAVAIL A FOURNIR

Conformément aux directives, les éléments suivants doivent être soumis :

- [CODE SOURCE] : Implémentation du parsing, de l'exécution et de l'historique.
- [MAKEFILE]    : Script pour la compilation automatisée du programme.
- [DOC TECHNIQUE]: Description de l'architecture, des choix d'implémentation et des algorithmes.
- [USER GUIDE]   : Guide d'utilisation détaillé (rédigé en anglais).

---

## 4. INSTALLATION ET COMPILATION

$git clone https://github.com/4bdOps/MINI-SHELL$ cd mini-shell
$ make
$ ./mini_shell

---

## 5. EQUIPE DE DEVELOPPEMENT

- Membre 1 : abdellah yeoussoumass --> PARSER 
- Membre 2 : asmae elghazi         -->HISTORY AND ERRORS HANDLING
- Membre 3 : fairouz mhijeb        -->BUILT-IN CMNDS
- Membre 4 : fadi safou            -->REDIRECTION AND PIPS
- Membre 5 : moncef essaken        -->SCRIPTS AND DOCUMENTATION

Structure : teamwork

