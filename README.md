SpaceCraft
===

Génération procédurale d'univers. Chaque planète représente un voxel:

* [X] Planète avec relief
* [ ] Grottes
* [ ] Système de climats/Biômes
* [ ] Systèmes solaire
* [ ] Collision avec le monde

##Description

##Dépendances

Les libs fournis dans extlibs sont x64 sauf pour celles de windows car 
Les libraries à installer sont:

- glfw3
- AntTweakBar
- libnoise
- SFML 2.1

##Compilation

- UNIX
```
cmake .
make
```
- Windows
Chez windows les libraries fournis peuvent poser des soucis à l'édition des liens. Pour que tout fonctionne correctement on conseille de les installer en les compilant soi même et des les mettre dans le dossier de MinGW. Il faut également ajouter le dossier de `include` et `bin` à la ligne 27 et 28:

```
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} C:/CodeBlocks/MinGW/lib ${PROJECT_SOURCE_DIR}/extlibs/libs-win32)
  set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} C:/CodeBlocks/MinGW/include )
```


```
cmake -G "MinGW Makefiles" .
mingw32-make
```

Finalement il faut copier les dll correspondantes dans le dossier bin

##Exécution

- Windows: Il faut copier les fichiers se trouvant dans `extlibs/dll` dans le dossier bin puis lancer `.\bin\spacecraft.exe`
- Linux: `./bin/spacecraft`
- OS X: `./bin/spacecraft`

##Tests

Les tests unitaires sont lancés avec `make test` il faut cepedant avoir fait `make all` avant car CTest ne compile pas les test (c'est un bug qui sera sûrement corrigé). En résumé

```
cmake .
make
make test
```

Ces tests permettent de vérifier les focntionalités des différentes classes/modules et ainsi aassurent le développement par incréments.

##Licence

TODO
