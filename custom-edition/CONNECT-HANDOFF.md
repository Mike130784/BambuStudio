# Huidige plaat openen in Bambu Connect

## Gebruik

1. Installeer de officiële Windows-app [Bambu Connect](https://wiki.bambulab.com/en/software/bambu-connect) en meld zelf aan. De geteste versie is **2.5.0-beta.15** met geldige Authenticode-handtekening.
2. Slice de gewenste plaat in onze custom build. Kies **Bestand → Exporteren → Open huidige plaat in Bambu Connect...**. De optie is uitgeschakeld zolang de huidige plaat geen geldige slice heeft.
3. Bevestig in Connect **Import Gcode 3MF**. Controleer model/printplaat, P2S, nozzle en filament. **Print** opent het voorbereidingsscherm met printer, AMS-toewijzing en opties; **Send** verstuurt de opdracht en start het printproces. Dat laatste is een eigen gebruikersactie.

Deze integratie opent alleen de huidige geslicede plaat in de officiële afzonderlijke Connect-app. De bestaande rechtstreekse Printplaat-functie is niet vervangen. Onze app blijft NotSigned; deze wijziging geeft de eigen build geen Bambu-identiteit en heft de oorspronkelijke cloudcamera-403 of directe printblokkade niet op. De lokale camerakeuze blijft apart beschikbaar.

## Implementatie en grenzen

Gebaseerd op Bambu's publieke import-file-contract: `bambu-connect://import-file` met geëncodeerde absolute `path`, `name` en vaste `version=1.0.0`. Eigen code verzorgt UTF-8/encodeURIComponent-compatibele codering. Er wordt geen externe implementatie overgenomen, geen shell gestart, geen token toegevoegd en geen controle van de gesloten netwerkplug-in verwijderd.

De huidige plaat gaat door de bestaande native export_3mf met dezelfde sliced-exportvlaggen als de gewone export. Elke overdracht maakt een unieke lokale map onder `<datadir>/connect-exports`, met een eigen `plate.gcode.3mf`. Kopieën blijven behouden omdat Connect ze asynchroon leest; automatisch cachebeheer is nog niet toegevoegd. Er wordt niets over een eerdere opdracht heen geschreven. Een ontbrekende Windows-protocolregistratie of mislukte export/start geeft een melding. Alleen Windows is geïmplementeerd en gebouwd.

## Bewijs op 5 november 2026

- Windows-build/install PASS; eigen DLL SHA256 `3D2BF297F5D0EFA91A37FD98584688F809F23E02E07026EC9306C7B7540394D8`, Authenticode: NotSigned.
- Native MSVC-contractproeven **11 PASS**: officieel voorbeeld, Windows-pad, Unicode, gereserveerde tekens, queryscheiding en ongeldige invoer. Werkelijke Unicode-import in Connect is NOT RUN.
- GUI-menu vóór slicing uitgeschakeld; na slicing beschikbaar: PASS. Testplaat P2S 0.4, 25 lagen, 1,03 g, 9m44s.
- Eerste importbevestiging geannuleerd; tweede overdracht met aparte kopie opnieuw aangeboden en geïmporteerd: PASS. Beide pakketten hebben goede ZIP-CRC en G-code-MD5; hun G-code is gelijk.
- Connect toont de juiste modelpreview, P2S, Textured PEI Plate, PLA, 0.4 en 1,03 g. De tijdweergave rondt af naar 10m. Dit verandert de 9m44s in G-code niet.
- Send to print-scherm toont printer en vier AMS-slots plus extern filament. Alleen de toewijzing in de voorbereiding is A2 naar A1 gewijzigd; daarna Cancel. Geen materiaalwissel of printopdracht uitgevoerd.
- Opgeslagen project na overdracht behoudt mesh, eenheden, scèneverwijzingen/-transformaties, één object en alle projectinstellingen; UUID-only metadata uitgezonderd. Oorspronkelijk referentiebestand bytegelijk aan de eerdere hash.
- Normale appconfiguratie heropend: lokale camerakeuze behouden en Loading naar Playing. Donkere scène; beweging/duurtest niet opnieuw geclaimd.

[Geschoond buildbewijs](evidence/connect-acceptance-2026-11-05.json). Installer en binaries, projectbestanden, screenshots, account-/apparaatgegevens en toegangscodes worden niet naar GitHub geüpload.

**NOT RUN:** Send/upload, echte printstart en voltooiing via Connect, fysieke AMS-wissel, meerdere platen/materialen, mislukte export/ontbrekende installatie in runtime, andere besturingssystemen en volledige editorveiligheidssuite opnieuw. De eerste echte print is een afzonderlijk door de gebruiker gekozen en gestarte proef. De integratie en voorbereiding zijn bewezen; een voltooide hardwareprint nog niet.

## Contracttest bouwen

Vanuit Developer PowerShell in deze bronmap: `cl /nologo /std:c++17 /EHsc /utf-8 custom-edition/tests/test-connect-handoff.cpp /Fe:<eigen-testmap>/test-connect-handoff.exe /Fo:<eigen-testmap>/test-connect-handoff.obj`. Voer daarna die eigen test-EXE uit. Deze test maakt alleen strings en verstuurt niets.
