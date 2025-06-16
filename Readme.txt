Damit das Programm funktioniert, muss WinFsp installiert sein:
https://winfsp.dev/rel/

Wenn ihr es im Standardpfad installiert habt, ist die winfsp-x64.dll dort zu finden:
C:\Code\WinFsp\bin\winfsp-x64.dll

Hier eine Anleitung für die config.txt:

# Konfiguration für RamCache
RamCache rc1 8 2
# rc1: RamCache mit 8 GB und caching nach 2 Lesezugriffen

# Konfiguration für SsdCache
SsdCache sc1 E: 8 10
# sc1: SsdCache auf Laufwerk E: mit 8 GB und caching nach 10 Lesezugriffen

# Kombination der Caches
Cache c1 sc1 rc1
# c1: Kombination von sc1 (SsdCache) und rc1 (RamCache)

# Wichtig:
# Wenn bei einem Cache die Anzahl der Lesezugriffe auf 0 gesetzt wird,
# wird jeder Zugriff sofort zwischengespeichert.
# Steht dieser Cache vorne in der Kombination,
# werden alle nachfolgenden Caches ignoriert und nicht mehr verwendet.

# Virtuelle Festplatten-Konfiguration
Vhdd F: G: rc1
# F: ist die originale Festplatte, die auf G: gespiegelt wird. Der Cache (rc1) wird auf G: angewendet.

Vhdd D: H: rc1
# D: ist die originale Festplatte, die auf H: gespiegelt wird. Der Cache (rc1) wird auf G: angewendet.

# Logging-Konfiguration
Log 1 log.txt
# Log-Level: 1–6 (1 = kein Logging, 6 = sehr ausführlich).
# 2. Argument (optional): Dateiname für Log-Ausgabe. Wenn angegeben, wird dort geloggt, sonst nur Konsole.

# Konfiguration für Terminalanzeige
HideTerminal true
# true: Terminalfenster wird beim Programmstart ausgeblendet. false: Terminal bleibt sichtbar.