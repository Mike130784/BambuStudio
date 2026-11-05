# GUI-veiligheidsproeven

Gebruik de aparte testdatamap en eigen modellen. Bewaar eerst een referentieproject met één kubus en de gekozen P2S 0.4 / 0.20mm Standard / Bambu PLA Basic-instellingen. Geen printerbediening nodig.

1. Kopieer `parametric-block.scad` en zijn schema naar een eigen testmap; open die kopie in de editor. Breedte −1 moet worden geweigerd, met plaatsen uitgeschakeld.
2. Render breedte 30: verwacht 30×10×5 mm. Wijzig breedte naar 31 zonder te genereren: plaatsen moet meteen uitgeschakeld zijn. Genereer opnieuw om een nieuwe geldige uitvoer te krijgen.
3. Voeg na een geslaagde render een commentaarregel aan de bronkopie toe. Klik plaatsen: de editor moet weigeren en opnieuw genereren eisen. Herstel de eigen kopie vóór de volgende proef.
4. Wijzig na een geslaagde render het schemasbestand, bijvoorbeeld door extra witruimte toe te voegen. Plaatsen moet ook nu geweigerd worden. Herstel de kopie.
5. Open `cancel-bores.scad` met zijn schema. Dit eigen model heeft veel kleine boringen om de render lang genoeg actief te houden. Start genereren en klik tijdens de actieve render op **Cancel rendering**. Verwacht: annulering gemeld, plaatsen uitgeschakeld, plaat ongewijzigd en alleen de eigen renderprocessen gestopt.
6. Start die render opnieuw en sluit de editor terwijl hij actief is. Het hoofdvenster moet bruikbaar blijven, de plaat moet ongewijzigd zijn en er mag geen eigen renderer overblijven. Als de render al klaar was, bewijst die poging geen sluiten-tijdens-render; herhaal alleen als nodig.
7. Sla het bestaande project onder een nieuwe naam op. Vergelijk geometrie, objectaantal, build-transformaties en de volledige projectconfiguratie met de referentie, niet alleen een screenshot.
8. Begin een lege plaat. Open het gewone blokmodel, genereer breedte 30 en plaats precies één keer. Slice en bewaar project plus geslicede 3MF. Controleer 30×10×5 mm, één object en 25 lagen. De lokale proef gaf 1,03 g, 2m42s modeltijd en 9m44s totaal; tijden zijn schattingen voor de vastgelegde profielen.

Deze stappen zijn op 5 november 2026 door de agent uitgevoerd en geslaagd. De 60s-time-out en extra GUI-foutscenario's staan nog open; zie het checkpoint. Het boormodel garandeert niet op elke machine een renderduur van meer dan 60 s en is dus op zichzelf geen betrouwbare time-outproef.
