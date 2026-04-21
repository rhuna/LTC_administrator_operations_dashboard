# LTC Administrator Operations Dashboard v38

This snapshot keeps the full v36 operations stack intact and adds a calmer, cleaner UI polish pass.

## v38 highlights
- simplified navigation labels
- cleaner dashboard hierarchy
- summary strip for quick scanning
- reduced dashboard clutter without removing modules
- preserved scroll-friendly layout for smaller screens

## Build on Windows (Qt 6.11 MinGW)
```bat
set QT_PREFIX=C:\Qt\6.11.0\mingw_64
set MINGW_BIN=C:\Qt\Tools\mingw1310_64\bin
build_release_and_run.bat clean
```


## v38
- Added Search & Filters workspace with module, status, and keyword filtering across key operational tables.
