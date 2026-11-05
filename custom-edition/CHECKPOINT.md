# Checkpoint — donderdag 5 november 2026

## Nieuw geverifieerd

De op 4 november gebouwde app is opnieuw gebruikt, na controle van de DLL-hash. De functionele broncode is ongewijzigd sinds commit `2f05bc7a8e4c089a0adbe50363062872bb74fa97`. Er was geen nieuwe build nodig. Alle hieronder genoemde GUI-proeven zijn vandaag door de agent uitgevoerd; de handmatige acceptatie van gisteren blijft afzonderlijk als USER PASS geregistreerd.

| Proef | Resultaat |
|---|---|
| Breedte −1 | PASS: foutmelding, plaatsen uitgeschakeld |
| Breedte 30 renderen | PASS: 30×10×5 mm |
| Breedte na render veranderen naar 31 | PASS: oud resultaat direct niet plaatsbaar |
| Bronbestand na render veranderen | PASS: plaatsactie geweigerd; opnieuw genereren vereist |
| Schema na render veranderen | PASS: plaatsactie geweigerd; opnieuw genereren vereist |
| Actieve render annuleren | PASS: geen plaatsactie, nul achtergebleven OpenSCAD-processen |
| Editor sluiten tijdens actieve render | PASS: hoofdvenster blijft bruikbaar, nul achtergebleven OpenSCAD-processen |
| Bestaande kubus na alle fout-/annuleerproeven | PASS: geometrie, build-transformaties en alle projectinstellingen gelijk; één object |
| Parametrisch blok op lege plaat plaatsen en slicen/exporteren | PASS: één object, 30×10×5 mm, 25 lagen, 1,03 g, 9m44s totaal; CRC/MD5 goed |
| Ontbrekende renderer | PASS: duidelijke melding, plaatsen geblokkeerd |
| Syntaxisfout en lege SCAD-geometrie via echte OpenSCAD | PASS: foutafhandeling, geen STL/plaatsactie en geen achtergebleven renderer |
| Echte GUI-deadline met gecontroleerde wachtprobe | PASS: beëindigd na 60,097 s; log leeg; geen STL; plaatsen geblokkeerd |
| Bestaand parametrisch project na deze extra fouten | PASS: mesh, units, component-/buildreferenties en transformaties, objectaantal en alle projectinstellingen gelijk; UUID-only metadata uitgezonderd |

Zie [bewijsregistratie](evidence/acceptance-2026-11-05.json) en [reproduceerbare teststappen](GUI-CHECKS.md). De eigen proefmodellen gebruiken geen externe dependencies. De oorspronkelijke baseline en zijn uploads blijven apart bewaard. Er zijn geen printeropdrachten verzonden.

## Volgende stap

1. De vier extra GUI-foutscenario's zijn nu afgerond; zie [aanvullend foutenbewijs](evidence/failure-acceptance-2026-11-05.json). De time-out gebruikte een transparante testprobe, geen echte OpenSCAD-render; de GUI-timer, processtarter en beëindiging zijn de normale productieroute.
2. Project-/plaatwijziging via een andere instantie blijft NOT RUN. De dedicated testconfiguratie heeft `single_instance=false`, zodat die route niet is aangeboden; dezelfde-windowwijzigingen worden door de modale editor geblokkeerd. Test de andere-instantieroute later expliciet met een afzonderlijke configuratie en behoud de importbewaking.
3. Pak concrete fouten aan vóór uitbreiding. Daarna volgen het onthouden van renderer/modelpad, meer parametertypes en een gedocumenteerde Customizer-subset volgens de roadmap. De huidige proef vereist nog een `.scad.json`-schema en getalparameters.
4. Echte P2S-/AMS 2 Pro-verbinding, slots, materiaalwissels en eventuele testprint blijven apart en NOT RUN. Een printopdracht vraagt een afzonderlijke expliciete opdracht.

Een printergerichte custom release is nog niet gekwalificeerd. Bewaar de ongewijzigde baseline; geen reset/clean/force-push of stil conflict oplossen.

## Lokale hervathulp

- Branch: `custom/parametric-editor-v2.9.1`; fork `Mike130784/BambuStudio`; upstream `bambulab/BambuStudio`.
- Bronwerkmap: `outputs/BambuStudio-Custom/source-release` in de oorspronkelijke Codex-projectmap.
- Origineel bewaard: `D:\bsc291\baseline-original`; custom app: `D:\bsc291\custom-install\bambu-studio.exe`.
- Aparte custom testdata: `D:\bsc291\custom-data`; startbestand in lokaal project `Start-Custom-Test.cmd`.
- Volledige lokale evidence: `outputs/BambuStudio-Custom/evidence`. Nieuwe bestanden: `editor-acceptance-20261105.json`, `gui-safety-preservation-20261105.json`, `gui-cancel-20261105.json`, `gui-close-20261105.json`, `safety-after-cancel-20261105.3mf`, `parametric-block30-p2s-20261105.3mf` en bijbehorende `.gcode.3mf`.
- De computerbediening werkte vandaag via screenshotcoördinaten en toetsenbord. AX-elementacties gaven nog cached-element-fouten; gebruik dat niet als appcrashdiagnose. OpenSCAD werd afzonderlijk gekozen, niet meegebundeld. Binaries zijn Authenticode: NotSigned.

Machinepaden zijn een hervathulp, geen vereisten voor een nieuwe clone. Lokale instellingen, caches, plug-ins, binaries en originele ZIP-uploads blijven buiten Git.
