stencc - a compiler for the StenC domain-specific language
==========================================================

Dépendances
===========

Aucune.

Compilation et tests
====================

La compilation se fait tout simplement avec :
```sh
$ make
```

Il est possible de lancer une suite de tests avec la cible suivante :
```sh
$ make test
```

La suite de tests exécute les scripts présents dans le répertoire **tests/**.

Le script `check_all.sh` utilise `grep` pour afficher les fonctions dangereuses
du code comme `memcpy` ou les allocations dynamique. Ceci facilite
l'inspection manuel de ces fonctions.

Le script `valgrind.sh` lance `stencc` à travers `valgrind` pour détecter
les fuites de mémoire.

Le script `main_test.sh` lance `stencc` pour chaque fichier source présent
dans le dossier **code/**, il affiche un succès si la compilation passe. Puis il
lance `stencc` pour chaque fichier erroné dans **code/error/** et affiche un
succès si l'erreur est correctement détectée.

Utilisation
===========

```sh
$ ./stencc -h
usage: ./stencc < code.sten
       ./stencc file ...
       ./stencc -h
```

Le compilateur va lire les sources sur son entrée standard si aucun argument
n'est passé :
```sh
$ ./stencc < code/elif.sten
```

Si des fichiers sont passés en arguments `stencc` va les compiler un par un :
```sh
$ ./stencc code/do.sten code/for.sten code/while.sten
```

En sortie `stencc` affiche l'arbre de syntaxe abstrait du code, la table des
symboles et enfin les instructions MIPS correspondantes.

L'architecture du projet est la suivante :

- **code/** contient des fichiers sources compilables par `stencc` et utilisés lors des tests,
- **doc/** contient la documentation de MIPS,
- **tests/** contient les scripts,
- **asm_mips.c** code relatif à la génération de code MIPS,
- **ast.c** code relatif à l'arbre de syntaxe abstraite,
- **grammar.y** code relatif à l'analyse syntaxique,
- **lex.c** code relatif à l'analyse lexicale,
- **main.c** main,
- **quad.c** code relatif aux quads intermédiaires,
- **sym.c** code relatif à la table des symboles,
- **typedefs.h** contient les définitions de tous les types et structures utilisés.

Fonctionnalités
===============

- Le code est modulaire.

- Pour éviter une dépendance au linkage, l'analyse lexicale a été écrite à la
main pour ne pas dépendre de `(f)lex`.

- La table des symboles est implementée à l'aide d'un dictionnaire.

- Les quads du code intermédiaire sont stockés dans une liste chaînée.

- L'analyse syntaxique construit un arbre de syntaxe abstrait.

- Toutes les structures de contrôle (`for`, `if`, `else`, `while`, `do`)
sont implémentées.

- Les conditions sont générées en code court-circuit.

- Les opérateurs unaire (`+` et `-`) et binaire (`+`, `-`, `*`, `/`,
`%`, `|`, `&` et `^`) sont implémentés.

- Les tableaux sont quasiment implémentés mais ne sont pas fonctionnels
(voir le fichier **code/init.sten**).

- La génération de code MIPS produit du code exécutable par `spim`.

Critiques
=========

- Le code de ce projet n'est pas encore suffisament mature pour être
pleinement exploité.

- Le code n'est pas encore complètement commenté.

- Le front-end du compilateur est plus abouti et mieux testé que le back-end
qui a été écrit plus rapidement par défaut de temps.

- Il y a encore des fuites mémoires (!) détectées par `valgrind`.

- Il reste des warnings shift/reduce dans `yacc`.

- Le temps a manqué pour réaliser entièrement ce projet avec les objectifs
fixés au départ, une semaine supplémentaire aurait probablement permis
d'atteindre tous ces objectifs.
