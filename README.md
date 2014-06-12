SPACECRAFT
===

Génération procédurale d'univers.

* [X] Planète avec relief
* [X] Grottes
* [ ] Système de climats/Biômes
* [ ] Systèmes solaire
* [X] Collision avec le monde

##Description

SPACECRAFT a pour but de générer des galaxies qui soient visitables à l'échelle humaine. En utilisant du bruit de Perlin on génére des planètes uniques qvec des biômes et des grottes qui varient. Les modifications faites sur une planète sont conservées. 

##Dépendances

Les libs fournies dans extlibs sont x64 sauf pour celles de windows qui sont x86. Il est cependant conseillé de les installer soit même.

Les libraries à installer sont:

- glfw3
- AntTweakBar
- libnoise
- SFML 2.1

##Compilation

On conseille de créer un dossier build ou faire ` cmake ..`. Ainsi si CMake pose problème on peut toujours faire `rm -rf build` pour regénérer le projet.
On peut compiler en debug ou en release selon les besoins. Par exemple pour tester avec valgrind on va plutôt utiliser la version debug:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
```

Par défaut on compile en release, c'est équivalent à:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE ..
```

- **UNIX**
```
make run
```

- **Windows**:
Les libraries fournies peuvent poser des soucis à l'édition des liens. Pour que tout fonctionne correctement on conseille de les installer en les compilant soi même et des les mettre dans le dossier de MinGW. Il faut également ajouter le dossier de `include` et `bin` à la ligne 27 et 28:

```
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} C:/CodeBlocks/MinGW/lib ${PROJECT_SOURCE_DIR}/extlibs/libs-win32)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} C:/CodeBlocks/MinGW/include )
```


```
cmake -G "MinGW Makefiles" ..
mingw32-make run
```

Finalement il faut copier les dlls correspondantes dans le dossier bin. Cela est nécessaire si les dlls fournies ne marchent pas sur votre système.

##Exécution

```
make run
```

ou

```
./bin/spacecraft
```

##Tests

Les tests unitaires sont lancés avec `make test` il faut cepedant avoir fait `make all` avant car CTest ne compile pas les tests. En résumé:

```
make
make test
```

Ces tests permettent de vérifier les focntionalités des différentes classes/modules et ainsi aassurent le développement par incréments.

##Licence

TODO
