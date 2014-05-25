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
