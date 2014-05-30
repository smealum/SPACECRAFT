SpaceCraft
===

Short desc

##Description

##Dépendances

##Compilation
Les libs fournis dans extlibs sont x64 sauf pour celles de windows car 

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

Les libraries à installer sont:

- glfw3
- AntTweakBar
- libnoise

```
cmake -G "MinGW Makefiles" .
mingw32-make
cp extlibs\dll\AntTweakBar.dll bin
```

##Exécution

- Windows: Il faut copier les fichiers se trouvant dans `extlibs/dll` dans le dossier bin puis lancer `.\bin\spacecraft.exe`
- Linux: `./bin/spacecraft`
- OS X: `./bin/spacecraft`

##Licence
