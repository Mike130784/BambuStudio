# Hervatten op donderdag 5 november 2026

## Afgerond

De eerste lokale parametrische editor is gebouwd en handmatig geaccepteerd: getalparameter wijzigen, 30×10×5 mm genereren, één model toevoegen en Undo met behoud van de bestaande kubus. De gebruiker heeft alle vier stappen bevestigd. Daarna bevestigt een screenshot dat de oorspronkelijke kubus in de custom app dezelfde zichtbare sliceresultaten geeft: 100 lagen, 3,63 g en 15m53s totaal.

De originele baseline en custom build staan apart. Er is nog geen printergerichte release of installer gepubliceerd. De Windows-app gebruikt dezelfde upstream-versienummering; de custom functie is herkenbaar aan het extra editor-menu. Zie de DLL-hashes in de bewijsregistratie om de twee lokale builds te onderscheiden.

## Morgen eerst

1. Gebruik de bestaande custom testbuild met de aparte testdatamap; lees eerst dit checkpoint en controleer branch/werkmap. Een nieuwe volledige build is alleen nodig bij bronwijzigingen of een concreet verificatiedoel.
2. Test in de editor een ongeldige parameter, gewijzigde parameters na een geslaagde render en gewijzigde bron/schema: foutmelding of opnieuw genereren vereist, geen oud resultaat plaatsen en geen onverwachte plaatwijziging.
3. Test annuleren en sluiten tijdens een langere render: alleen de eigen rendererboom stopt, geen crash en geen toegevoegd model. Native proceslevensduurtests zijn al PASS; deze GUI-flow blijft apart te bewijzen.
4. Genereer het 30×10×5 mm-blok opnieuw, plaats het op een lege plaat of rangschik het naast bestaande objecten en slice met P2S 0.4 / 0.20mm Standard / Bambu PLA Basic. Bewaar project en slice-uitvoer.
5. Noteer PASS/FAIL/NOT RUN en uitvoerder per test. Pak concrete fouten aan voordat meer parametertypes of modelondersteuning worden toegevoegd.

Live printer-/AMS-tests zijn een volgende fase; geen printopdracht zonder afzonderlijke expliciete opdracht. Presetbeheer en een algemene Customizer-parser blijven op de roadmap.

## Lokale werkomgeving bij dit checkpoint

- Bronwerkmap: `outputs/BambuStudio-Custom/source-release` in de oorspronkelijke Codex-projectmap.
- Branch: `custom/parametric-editor-v2.9.1`; fork: `Mike130784/BambuStudio`; upstream: `bambulab/BambuStudio`.
- Bewaarde originele build: `D:\bsc291\baseline-original`; oorspronkelijke installatie ook in `D:\bsc291\install`.
- Custom app: `D:\bsc291\custom-install\bambu-studio.exe`; aparte testdata: `D:\bsc291\custom-data`.
- Startbestand in lokaal project: `Start-Custom-Test.cmd`.
- Volledige lokale logs, audits en hashes staan buiten de Git-repository in `outputs/BambuStudio-Custom/evidence`.
- Computerbediening gaf FrameArrived-timeouts en cached-element-fouten. De app bleef reageren; de gebruiker heeft de GUI-proeven uitgevoerd. Verwar die gereedschapsfout niet met een bewezen appcrash.

Deze machinepaden zijn een lokale hervathulp, geen vereisten voor een nieuwe clone. Houd lokale instellingen, caches, plug-ins, binaries en originele ZIP-uploads buiten commits. Geen reset/clean/force-push of stil conflict oplossen. Bewaar de ongewijzigde baseline.
