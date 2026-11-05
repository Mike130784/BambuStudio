# Beperkte Customizer-ondersteuning

Een zelfstandig `.scad`-bestand kan zonder `.scad.json` worden geopend als zijn beginblok aan deze expliciete subset voldoet. De reader is eigen code; hij voert geen SCAD uit en is geen algemene OpenSCAD-parser.

```scad
/* [Maten] */
// Breedte
width = 20; // [1:1:200]
depth = 10; // [1:200]
height = 5; // [1:200]
/* [Hidden] */
cube([width, depth, height]);
```

- Lees uitsluitend opeenvolgende declaraties aan het begin, tot de eerste geometrie-, module- of functie-instructie. Eén declaratie per regel, met een numerieke literal en een verplicht expliciet bereik `[min:max]` of `[min:step:max]` in het afsluitende regelcommentaar.
- Ondersteun decimalen met een punt, negatieve waarden en exponentnotatie. Minimaal één en maximaal 32 parameters; namen uniek, maximaal 64 tekens; getallen eindig en binnen ±1.000.000. Stapgroottes moeten positief zijn; standaardwaarde en invoer moeten op het raster vanaf het minimum liggen (numerieke tolerantie 1e-7 stap).
- Het direct voorafgaande korte regelcommentaar wordt als label gebruikt. Groepmarkeringen worden gelezen, maar nog niet als tabs weergegeven. `[Hidden]` en `$`-variabelen blijven in de bron en krijgen geen invoerveld. Een volgende groep beëindigt Hidden.
- Geen tekst, bools, vectors, keuzelijsten, onbegrensde parameters of berekende zichtbare standaardwaarden. Niet-ondersteunde koppen worden met regelnummer geweigerd. Gebruik daarvoor voorlopig een expliciet schema of pas een eigen model aan. Includes/imports blijven geblokkeerd door de bestaande conservatieve controle.
- Een bestaand `.scad.json` heeft altijd voorrang. Een kapot schema valt nooit stilzwijgend terug op Customizer. Bestaande schema's zonder `step` blijven vrij instelbaar binnen hun bereik; optioneel `step` gebruikt dezelfde rastercontrole.

Bron en schema blijven onaangeroerd. Genereren gebruikt een private kopie en een OpenSCAD-parameterset. Gewijzigde bron of een toegevoegd/verwijderd/gewijzigd schema maakt renderen of plaatsen onmogelijk totdat het model opnieuw wordt geopend. Geen automatische render, import, print of MakerWorld-toegang.

Eigen voorbeelden: `samples/customizer-block.scad` en `samples/customizer-spacer.scad`. Beide hebben bewust geen sidecar. Hun teksten en geometrie zijn nieuw gemaakt voor dit project. De gangbare rangesyntaxis en Hidden-conventie zijn onder meer terug te vinden in de [OpenSCAD-projectdiscussie](https://github.com/openscad/openscad/issues/3617). Er is geen externe parsercode overgenomen.

## Controles opnieuw uitvoeren

Vanuit de root van de clone, in een Visual Studio x64 Developer Command Prompt:

```bat
cl /nologo /std:c++17 /EHsc custom-edition\tests\test-customizer.cpp /Fe:%TEMP%\test-customizer.exe /Fo:%TEMP%\test-customizer.obj
%TEMP%\test-customizer.exe custom-edition\samples\customizer-block.scad custom-edition\samples\customizer-spacer.scad
```

De standalone test controleert 24 gevallen inclusief de twee voorbeelden. De gewone Windows-appbuild compileert dezelfde header. Voor GUI-acceptatie: open beide voorbeelden zonder schema, controleer standaardwaarden en labels, wijzig een maat, genereer/plaats/slice. Probeer een waarde tussen twee stappen, verander de bron na render en voeg een schema toe na render: plaatsen moet geweigerd blijven. Controleer ook het bestaande JSON-voorbeeld als regressie. Noteer build-, native en GUI-bewijs afzonderlijk; een testprint blijft een aparte opdracht.
