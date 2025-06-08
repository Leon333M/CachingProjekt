Damit das Programm funktioniert, muss WinFsp installiert sein:
https://winfsp.dev/rel/

Wenn ihr es im Standardpfad installiert habt, ist die winfsp-x64.dll dort zu finden:
C:\Code\WinFsp\bin\winfsp-x64.dll

Jetzt habt ihr zwei Optionen:
Entweder ihr kopiert die winfsp-x64.dll in den Ordner, in dem sich die CachingProjekt.exe befindet,
oder
ihr bindet den Pfad C:\Code\WinFsp\bin\ in die Systemumgebungsvariablen ein.

Hier eine Anleitung für die config.txt:

# Konfiguration für RamCache
RamCache rc1 8 2
# rc1: RamCache mit 8 GB und caching nach 2 Lesezugriffen

# Konfiguration für SsdCache
SsdCache sc1 E: 8 10
# sc1: SsdCache auf Laufwerk E: mit 8 GB und caching nach 10 Lesezugriffen

# Kombination der Caches
Cache c1 rc1 sc1
# c1: Kombination von rc1 (RamCache) und sc1 (SsdCache)

# Virtuelle Festplatten-Konfiguration
Vhdd F: G: rc1
# F: ist die originale Festplatte, die auf G: gespiegelt wird. Der Cache (rc1) wird auf G: angewendet.

Vhdd D: H: rc1
# D: ist die originale Festplatte, die auf H: gespiegelt wird. Der Cache (rc1) wird auf G: angewendet.