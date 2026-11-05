# GUI-veiligheidsproeven

Gebruik de aparte testdatamap en eigen modellen. Bewaar eerst een referentieproject met één kubus en de gekozen P2S 0.4 / 0.20mm Standard / Bambu PLA Basic-instellingen. Geen printerbediening nodig.

1. Kopieer `parametric-block.scad` en zijn schema naar een eigen testmap; open die kopie in de editor. Breedte −1 moet worden geweigerd, met plaatsen uitgeschakeld.
2. Render breedte 30: verwacht 30×10×5 mm. Wijzig breedte naar 31 zonder te genereren: plaatsen moet meteen uitgeschakeld zijn. Genereer opnieuw om een nieuwe geldige uitvoer te krijgen.
3. Voeg na een geslaagde render een commentaarregel aan de bronkopie toe. Klik plaatsen: de editor moet weigeren en opnieuw genereren eisen. Herstel de eigen kopie vóór de volgende proef.
4. Wijzig na een geslaagde render het schemasbestand, bijvoorbeeld door extra witruimte toe te voegen. Plaatsen moet ook nu geweigerd worden. Herstel de kopie.
5. Open `cancel-bores.scad` met zijn schema. Dit eigen model heeft veel kleine boringen om de render lang genoeg actief te houden. Start genereren en klik tijdens de actieve render op **Genereren stoppen**. Verwacht: annulering gemeld, plaatsen uitgeschakeld, plaat ongewijzigd en alleen de eigen renderprocessen gestopt.
6. Start die render opnieuw en sluit de editor terwijl hij actief is. Het hoofdvenster moet bruikbaar blijven, de plaat moet ongewijzigd zijn en er mag geen eigen renderer overblijven. Als de render al klaar was, bewijst die poging geen sluiten-tijdens-render; herhaal alleen als nodig.
7. Sla het bestaande project onder een nieuwe naam op. Vergelijk geometrie, objectaantal, build-transformaties en de volledige projectconfiguratie met de referentie, niet alleen een screenshot.
8. Begin een lege plaat. Open het gewone blokmodel, genereer breedte 30 en plaats precies één keer. Slice en bewaar project plus geslicede 3MF. Controleer 30×10×5 mm, één object en 25 lagen. De lokale proef gaf 1,03 g, 2m42s modeltijd en 9m44s totaal; tijden zijn schattingen voor de vastgelegde profielen.

Deze stappen zijn op 5 november 2026 door de agent uitgevoerd en geslaagd. Ook de onderstaande aanvullende GUI-foutproeven zijn geslaagd; bewijs staat in `evidence/failure-acceptance-2026-11-05.json`.

## Extra rendererfouten

- Gebruik een bewust niet-bestaand rendererpad met een geldig model. Genereren moet een melding geven en plaatsen blijft uitgeschakeld.
- Kies de officiële OpenSCAD en `samples/syntax-error.scad` met het naastliggende schema. Dit bestand is bewust ongeldig. Genereren moet falen zonder STL of plaatsactie.
- Herhaal met `samples/empty-model.scad`. De bron is geldig maar bevat bewust geen geometrie. Ook dat geeft geen plaatsbaar resultaat.
- Sluit de editor en sla de bestaande plaat onder een andere naam op. `python custom-edition/tests/verify_project_preservation.py referentie.3mf na-fouten.3mf rapport.json` vergelijkt mesh, units, component-/buildreferenties en transformaties, objectaantal en alle projectinstellingen. Alleen UUID-metadata wordt genegeerd.

## Gecontroleerde 60s-deadline

Het boormodel garandeert niet op elke machine een renderduur van meer dan 60 s. Daarom gebruikt deze proef een kleine eigen wachtprobe. Dit is **geen OpenSCAD**, wordt niet met de app meegeleverd en bewijst geen geometriefunctionaliteit.

Compileer `custom-edition/tests/timeout-renderer-probe.cpp` vanuit Developer PowerShell: `cl /nologo /std:c++17 /EHsc custom-edition/tests/timeout-renderer-probe.cpp /Fe:<testmap>/timeout-renderer-probe.exe /Fo:<testmap>/timeout-renderer-probe.obj`. Gebruik een bestaande eigen testmap en echte absolute paden in plaats van de placeholders.

Kies de probe tijdelijk als renderer en een geldig model. De probe schrijft alleen een PID-marker in de unieke rendermap en wacht maximaal 90 s, zonder STL te produceren. De normale editor moet hem rond 60 s beëindigen, de deadlinefout tonen en plaatsen blokkeren. Meet processtart en proceseinde met een handle die vóór de beëindiging is geopend. Controleer dat het log leeg is en er geen STL bestaat: daarmee wordt de tijdslimiet onderscheiden van de loglimiet. De uitgevoerde proef mat **60,097 s**, exitcode 1 en nul logbytes. Kies na afloop weer de officiële OpenSCAD.


## Native interface en menutoegang

1. Open het rechtermuisknopmenu op een bestaand model. `Parametrisch model...` moet precies eenmaal bovenaan staan en dezelfde editor openen als Bestand.
2. Kies het voorbeeldmodel en OpenSCAD via de bestandskiezers. Controleer leesbare Breedte/Diepte/Hoogte, bereik en mm; alle knoppen moeten in het venster passen.
3. Sluit en heropen de editor, daarna de hele app. Beide paden moeten blijven staan, standaardwaarden 20/10/5 geladen, plaatsen uitgeschakeld en geen automatische render gestart.
4. Controleer de ingang op de lege printplaat en na herhaald openen van het menu op dubbele vermeldingen.
5. Controleer ongeldig getal, render 30x10x5, wijziging maakt oude uitvoer ongeldig, opnieuw genereren en eenmalig plaatsen. Slice met hetzelfde P2S-profiel; vergelijk 25 lagen/1,03 g/9m44s.
6. Probeer de nieuwe stopknop en sluiten tijdens actieve render; geen eigen renderer mag achterblijven. Dark mode en andere schermschalen zijn afzonderlijke runtimeproeven.


## Numerieke Customizer-subset

Zie [scope en reproduceerbare controles](CUSTOMIZER.md) en [buildspecifieke acceptatie](evidence/customizer-acceptance-2026-11-05.json). De geteste eigen voorbeelden hebben geen sidecar. De overige hoofdstukken blijven eerdere, afzonderlijk gedateerde evidence.


## Dark-modecorrectie

Controleer in dark mode de lege status, geladen modelvelden, een invoerfout, geslaagde render, voortgang/stoppen en de uitgeschakelde plaatsknop. Er mogen geen lichte native labelvlakken in de donkere statuskaart staan. De native Windows-voortgangsbalk is bewust behouden. Controleer daarnaast de lege editor in light mode. Buildspecifieke resultaten staan in [dark-modeacceptatie](evidence/dark-acceptance-2026-11-05.json); een live themawissel en andere DPI-schalen blijven apart.
