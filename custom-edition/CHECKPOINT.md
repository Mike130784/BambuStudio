# Checkpoint — donderdag 5 november 2026

## Nieuwste checkpoint: camera blijft geblokkeerd in eigen builds

Cloud-livebeeld geeft **FAIL: 1:403** in de custom build én in de apart bewaarde ongewijzigde upstream-build. Officiële Studio van dezelfde release toont livebeeld (USER PASS en agentwaarneming); Handy is USER PASS. Daarmee is de fout onafhankelijk van de editorwijzigingen gereproduceerd. Software-identiteit/autorisatie is een onderbouwde hypothese, geen bewezen specifieke oorzaak of opgeloste fout.

Zie [diagnose en vervolgstappen](CAMERA-DIAGNOSTIC.md) en [geschoond bewijs](evidence/camera-diagnostic-2026-11-05.json). Status en AMS-slots zijn alleen-lezen zichtbaar; volledige hardwarekwalificatie blijft open. Geen productcode of printerinstellingen gewijzigd. De bestaande print bleef doorgaan. Geen nieuwe build nodig voor deze documentatie. H07-livebeeld is een expliciete kwalificatieblokkade.

## Eerder checkpoint: dark mode hersteld

De editor gaf in donkere modus lichte Windows-achtergronden onder de statuslabels. De eigen StaticBox-tekenkleur was al donker, maar de native labelachtergrond niet. Die achtergronden worden nu expliciet gelijkgezet. Ook afgeronde hoeken en scheidingslijnen passen bij het thema. Succes-/foutkoppen hebben in dark mode lichtere tinten; uitgeschakelde knopteksten zijn beter leesbaar.

Windows-build/install **PASS**. Praktische GUI-controle van donkere neutrale status, geladen parameters, ongeldige stap, succes op 30x10x5 mm, actieve render/stoppen en uitgeschakelde plaatsknop is **PASS**. De lege editor is ook in lichte modus gecontroleerd: **PASS**. [Buildspecifieke registratie](evidence/dark-acceptance-2026-11-05.json). De native voortgangsbalk blijft de standaard Windows-balk; de problematische brede lichte statusvlakken zijn weg.

De normale custom-installatie is op verzoek bijgewerkt. Geteste DLL SHA256: `D1688AA69F215A47E890478D9A2C148144F4FF3B896A3487DCA31BF6DDF68953`; **Authenticode: NotSigned**. Bouwlog: `custom-20261105-141101-BuildApp.log`; installatie naar de gewone map: `dark-install-main.log`.

Bambu-account inloggen is afzonderlijk **USER PASS** op basis van expliciete gebruikersbevestiging; een aangemelde homepage is ook geobserveerd. Dit bewijst geen P2S/AMS-hardwarekwalificatie en is geen agent-uitgevoerde authenticatie. Accountinstellingen zijn niet aangepast. Geen accountdata, tokens of modelprojecten gepubliceerd.

Deze wijziging raakt uitsluitend editorweergave. Geen runner-, geometrie-, profiel- of accountlogica aangepast. Een live themawissel met geopend venster, andere DPI-schalen en een nieuwe volledige sliceregressie zijn **NOT RUN** voor deze kleurcorrectie. Echte P2S/AMS-praktijk en print blijven afzonderlijk.

Volgende functionele stap blijft booleans en vaste keuzelijsten met typecontrole.

## Eerder checkpoint: numerieke Customizer-kop zonder schema

Windows-build/install **PASS**; native MSVC-parserproeven **24 PASS**. De editor ondersteunt nu een [gedocumenteerde beperkte subset](CUSTOMIZER.md): numerieke begin-declaraties met `// [min:max]` of `// [min:step:max]`, korte labels, groepmarkeringen en Hidden. Er is geen algemene SCAD-parser of ondersteuning voor bool/tekst/keuze/vector toegevoegd. Externe dependencies blijven geblokkeerd; er is geen externe parsercode overgenomen.

Twee eigen voorbeelden zijn met echte OpenSCAD en via de GUI getest: blok naar 30x10x5 mm en afstandsring naar 30x30x6,5 mm. Stapcontrole weigert breedte 30,5 bij stappen van 1. Het blok is één keer geplaatst, gesliced en geëxporteerd met de vaste P2S-profielen: **25 lagen, 1,03 g, 9m44s**, CRC/MD5 PASS. Het bestaande JSON-model rendert nog correct.

Een toegevoegd schema na render blokkeert plaatsen. Een kapot bestaand schema valt niet terug op Customizer. Een niet-ondersteunde bool geeft een regelnummer en blokkeert genereren/plaatsen. Na alle proeven zijn geometrie, eenheden, scèneverwijzingen/-transformaties, objectaantal en alle projectinstellingen behouden; UUID-only metadata uitgezonderd. [Gedetailleerd bewijs](evidence/customizer-acceptance-2026-11-05.json).

- Geteste DLL SHA256: `3E69C34F1C6866C48AA6EA120435DBA7613FBB8BD50D37F7C32C08AB478562A4`; **Authenticode: NotSigned**.
- Bouwlog lokaal: `custom-20261105-135001-BuildApp.log`. Vorige geteste UI-build apart bewaard in `D:\bsc291\custom-ui-checkpoint-521c8e8`; origineel blijft apart bewaard.
- Nieuwe lokale projecten: `customizer-block30-p2s-20261105.3mf`, bijbehorende `.gcode.3mf` en `customizer-after-guards-20261105.3mf` in de evidence-map. Geen modelprojecten of binaries naar GitHub geüpload.
- Derdepartijcatalogi, dependencies, extra parametertypes, dark/DPI en echte P2S/AMS-praktijk blijven **NOT RUN**. De ongewijzigde runner/annulering en gewijzigde-broncontrole zijn bij eerdere builds getest; geen herhaling daarvan geclaimd op deze build.

Volgende afgebakende stap: booleans en vaste keuzelijsten ontwerpen met typecontrole; daarna heropenen van modelparameters vanuit een opgeslagen project. Volledige Customizer-compatibiliteit, meshbewerking en printerkwalificatie blijven afzonderlijke fasen.

## Eerder checkpoint: Bambu-interface en contextmenu

Nieuwe Windows-build/install **PASS**. De editor gebruikt native Bambu-knoppen en invoervelden, Nederlandstalige labels, bereikcontrole, leesbare eenheden, voortgang en een expliciete groene plaatsknop. `Parametrisch model...` staat nu in Bestand en bovenaan het rechtermuisknopmenu op een object, de printplaat en het lege werkvlak. Herhaald openen geeft geen dubbele vermelding. Het maakt een nieuw SCAD-model; een geselecteerde STL wordt niet automatisch parametrisch of vervangen.

Model- en OpenSCAD-pad worden lokaal in de eigen AppConfig-sectie bewaard. Beide paden en de standaardparameters zijn na een volledige appherstart teruggevonden. Er start geen automatische render of plaatsactie. Bestandskiezers, ongeldige invoer, genereren op 30x10x5 mm en ongeldig maken na parameterwijziging zijn **PASS**.

Eenmalig plaatsen en P2S-slicen/exporteren zijn opnieuw **PASS**: één model van 30x10x5 mm, 25 lagen, 1,03 g, 2m42s modeltijd en 9m44s totaal. ZIP-CRC en G-code-MD5 zijn goed. Stoppen en sluiten tijdens echte actieve OpenSCAD-renders zijn opnieuw **PASS**, met nul resterende rendererprocessen. Opgeslagen plaat na deze proeven behoudt geometrie, transformaties, eenheden, objectaantal en alle projectinstellingen (UUID-only metadata uitgezonderd).

- Geteste DLL SHA256: `439EDD1A1ADCBB64D11801AC71DE2A5512C1E9DDC7F9AB1BE0C850E116F462C2`; **Authenticode: NotSigned**.
- Lokaal bouwlog: `custom-20261105-115238-BuildApp.log`. Bronhashes en bewijs: [UI-acceptatie](evidence/ui-acceptance-2026-11-05.json).
- Originele baseline blijft bewaard. De vorige werkende custom versie staat daarnaast in `D:\bsc291\custom-before-ui-20261105`.
- Lichte modus op de huidige schermschaal is visueel getest; donker thema en extra DPI-schalen zijn **NOT RUN**. Hardware P2S/AMS en echte print blijven **NOT RUN**.
- Basale modelbewerking is als aparte toekomstige M1-M4-stappen opgenomen in [de roadmap](ROADMAP.md); nog geen implementatie.

Volgende concrete stap: representatieve eigen SCAD-modellen en een beperkte Customizer-subset voorbereiden, met dezelfde controles en projectbehoud. Printerkwalificatie blijft afzonderlijk.

## Eerdere proeven op de eerste editorbuild

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
3. De interface en het onthouden van paden zijn nu afgerond; zie het nieuwste checkpoint bovenaan. Daarna volgen meer parametertypes en een gedocumenteerde Customizer-subset volgens de roadmap. De huidige proef vereist nog een `.scad.json`-schema en getalparameters.
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
