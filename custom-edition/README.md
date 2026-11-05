# Bambu Studio Custom Edition — lokale editorproef

Eigen ontwikkelbranch op Bambu Studio **2.9.1 / 02.09.01.70**, basiscommit `56a57d00a591360cf41bf21383915ee5bf357be5`. De oorspronkelijke AGPLv3-licentie blijft van toepassing; er is geen officiële Bambu-release of goedkeuring voor deze fork geclaimd.

**Bestand → Parametrisch model…** (ook via rechtermuisknop op een model of printplaat) opent de eerste lokale parametrische editor. Kies een eigen officiële OpenSCAD-installatie en een zelfstandig SCAD-bestand met een naastliggend `.scad.json`-schema. De voorbeelden staan in [samples](samples/). Wijzig `width` van 20 naar 30, genereer lokaal en plaats het model expliciet op de plaat. De bestaande importfunctie voegt alleen geometrie toe, met Undo.

Het venster gebruikt de native Bambu-stijl, Nederlandstalige teksten en duidelijke bereik- en resultaatmeldingen. Geldige modelpaden en de expliciet gekozen OpenSCAD worden lokaal onthouden, ook na herstart. Bij openen worden alleen de standaardparameters geladen; renderen en plaatsen vereisen een eigen klik. Een contextmenu start dezelfde editor en wijzigt het geselecteerde STL-model niet.

De eerste versie ondersteunt maximaal 32 getalparameters met bereikcontrole en optionele `label`/`unit`-metadata. Andere parametertypes, een algemene Customizer-parser en een volledige SCAD-teksteditor zijn nog niet geïmplementeerd. Afhankelijkheden (`include`, `use`, `import`, `surface`) worden conservatief geweigerd, ook wanneer de woorden in commentaar staan. Dit is een beperking van deze proef, geen volledige SCAD-parser.

De renderer draait asynchroon zonder shell, in een eigen Windows Job Object. Sluiten/annuleren beëindigt alleen de eigen procesboom. Time-out: 60 s; lokale loglimiet: 10 MB; STL-limiet: 20 MB; maximale afmeting: 1000 mm per as. Bron/schema/parameters en project/plaat worden gecontroleerd voordat een resultaat plaatsbaar is. Renderbestanden blijven lokaal in `<datadir>/parametric-cache`; er is nog geen automatisch cachebeheer. Open alleen vertrouwde modellen: OpenSCAD heeft de normale lokale bestandstoegang van de gebruiker.

## Status op 5 november 2026

- Originele Windows-build, appstart en P2S-kubus slice/export: **PASS**.
- Custom Windows-build/install en openen editor: **PASS**; lokale binaries **Authenticode: NotSigned**.
- Native runner: echte OpenSCAD 2021.01-renders van 20×10×5 en 30×10×5 mm; foutieve/lege modellen, ontbrekende renderer, quotes/backslashes, spatie/Unicodepaden en eigen procesboom stoppen: **PASS**.
- Genereren op 30×10×5 mm, één keer plaatsen en Undo met behoud van bestaande kubus: **USER PASS**.
- Kubus opnieuw slicen in de custom app: **USER PASS**. Screenshot toont dezelfde waarden als origineel: 100 lagen, 1,20 m / 3,63 g, 15m53s totaal. Dit is geen bytevergelijking van G-code.
- GUI-bereikcontrole, ongeldig maken na parameterwijziging en weigeren van gewijzigde bron/schema: **PASS**. Annuleren en sluiten tijdens een actieve render: **PASS**, met nul achtergebleven OpenSCAD-processen.
- Behoud bestaande plaat na deze proeven: **PASS**. Opgeslagen 3MF bevat dezelfde kubus, build-transformaties en alle projectinstellingen als de originele nulmeting.
- Parametrisch blok op een lege plaat genereren/plaatsen/slicen/exporteren: **PASS**. Exact één object van 30×10×5 mm, 25 lagen, 1,03 g en 9m44s totaal; export-CRC en G-code-MD5 gecontroleerd.
- Ontbrekende renderer, syntaxisfout en leeg model via de GUI: **PASS**. Echte GUI-time-out met een gecontroleerde wachtprobe: **PASS**, proces beëindigd na 60,097 s, geen STL en plaatsen geblokkeerd. De probe is geen OpenSCAD; beide echte SCAD-fouten zijn wel met OpenSCAD getest. Zie [aanvullend foutenbewijs](evidence/failure-acceptance-2026-11-05.json).
- Project-/plaatwissel via een andere instantie tijdens render: **NOT RUN**; de dedicated testconfiguratie heeft `single_instance=false`. De gewone modale editor blokkeert wijzigingen in het eigen hoofdvenster.
- Werkelijke P2S/AMS 2 Pro-verbinding, materiaalwissels, drogen en testprint: **NOT RUN**. Aanwezige bronondersteuning en een build bewijzen geen hardwarecompatibiliteit.

De nieuwe Bambu-interface, padherstel na herstart, contextmenu's en slice-/stopregressie zijn opnieuw **PASS**; zie [UI-bewijs](evidence/ui-acceptance-2026-11-05.json). Donker thema en extra DPI-schalen zijn nog **NOT RUN**.

Zie [actueel checkpoint](CHECKPOINT.md), [Windows bouwen](BUILD-WINDOWS.md), [roadmap en testcriteria](ROADMAP.md), [GUI-teststappen](GUI-CHECKS.md) en de bewijsregistraties van [4 november](evidence/acceptance-2026-11-04.json) en [5 november](evidence/acceptance-2026-11-05.json). De opgeslagen 3MF-/G-code-projecten blijven lokaal; hashes staan in de registratie.

## Herkomst en grenzen

De aangeleverde Bambu-ZIP (02.08.01.55) is afzonderlijk geverifieerd; de ontwikkelbranch gebruikt bewust de nieuwere officiële 2.9.1-release. De originele build is vóór editorwijzigingen apart bewaard. Alleen de GUI-editor, het menu en de bouwlijst zijn functioneel gewijzigd; deze map bevat de aanvullende projectdocumentatie en eigen voorbeelden/tests.

Helio Slicer Presets Studio 0.4.2 blijft inspiratie voor later presetbeheer: in de upload werd geen projectlicentie gevonden en er is geen code overgenomen. Materializr is alleen onderzocht als inspiratie; ook daaruit is geen code overgenomen. Er wordt geen toegang tot beschermde MakerWorld-cloudgeneratoren of automatisch distributierecht voor de niet-vrije Bambu-netwerkplug-in verondersteld. OpenSCAD wordt afzonderlijk aangeroepen en niet meegebundeld.
