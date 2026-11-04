# Roadmap en concrete acceptatie

| Fase | Werk | Poort naar volgende stap |
|---|---|---|
| P0a — bron | Archieven bewaren, hashes, officiële revisie, licentie-inventaris en branch | Volledige bronvergelijking PASS |
| P0b — Windows | Ongewijzigde dependencies en app bouwen, portable install, exe-hash en bouwlog | Build/install PASS; starten met aparte testconfiguratie PASS |
| P0c — functionele nulmeting | P2S/AMS-baseline vastleggen zonder editor | Offline slicing PASS; live printerhandelingen afzonderlijk vastgelegd |
| P1a — lokale renderer | Eigen SCAD+schema, getypeerde parameters, uitvoer en annuleren | Render- en fouttests PASS; nog geen plaatintegratie |
| P1b — geïntegreerde editor | wxWidgets-venster, bron/parameters, STL-preview/import, Undo | Verticale proef en regressiecriteria PASS |
| P1c — bestaande modellen | Customizer-subset, dependencies, heropenen parameters | Representatieve beschikbare SCAD-modellen gevalideerd |
| P2 — presets | Helio-rechten vaststellen; eigen alleen-lezen vergelijking/overerving | Nul wijzigingen aan bestaande profielen; betrouwbare diff |
| P2b — presetexport | Voorbeeldweergave, kopie/export, import met Bambu-validator | Cycli/ontbrekende ouders/ongeldige sleutels veilig afgehandeld |
| P3 — kwalificatie | Regressie en echte P2S + AMS 2 Pro-praktijk, verpakking | Alle vereiste tests PASS en distributie-inventaris afgerond |

Geen uitbreiding van P1 start voordat P0b een werkende originele build oplevert. Live hardwaretests blijven apart: ze kunnen niet door een compilatie worden vervangen. Tot P0c is afgerond blijft printercompatibiliteit onbewezen en komt er geen custom printrelease.

## Testmatrix

Gebruik steeds dezelfde voorbeeldprojecten, dezelfde profielen en dezelfde printerfirmware. Leg de originele baseline en de custom build naast elkaar vast. Noteer verwacht/werkelijk resultaat, log/screenshot, versie, datum en uitvoerder. Statuswaarden: PASS, FAIL, NOT RUN.

| ID | Test | Acceptatie |
|---|---|---|
| B01 | Bron en officiële revisie | Nul byteverschillen en nul ontbrekende/extra bronbestanden |
| B02 | Release x64 dependencies en app | Alle buildstappen exit 0; geen overgeslagen essentiële dependency |
| B03 | Portable app starten | Hoofdvenster opent; eigen testdatamap; normale installatie blijft bruikbaar |
| B04 | P2S 0.4 + 20 mm kubus | Slice/preview/export zonder fout; nozzle, bed en afmetingen correct |
| B05 | P2S nozzleprofielen | 0.2/0.4/0.6/0.8 selecteerbaar; passend proces zonder profielverlies |
| B06 | 3MF roundtrip en bestaande plaat | Openen, opslaan, heropenen behoudt objecten/platen en filamenttoewijzing |
| B07 | Nulmeting slicing | Zelfde input/config; vergelijk lagen, filament en tijd; onderzoek afwijkingen. Normaliseer alleen bekende tijdstempels/metadata bij bestanddiff |
| H01 | Netwerkplug-in en P2S-detectie | Versies/voorwaarden vastgelegd; echte P2S bereikbaar via gekozen toegestane route |
| H02 | AMS 2 Pro | App toont vier slots, materiaal/kleur en actuele beschikbaarheid correct |
| H03 | Filamentmapping | Twee materialen aan juiste AMS-slots gekoppeld; wissels zichtbaar in preview |
| H04 | Laden/lossen | Praktische test op echte printer; resultaat bevestigen met gebruiker |
| H05 | Drogen/vochtigheid | Alleen functies die de echte P2S/firmwarecombinatie aanbiedt; met baseline vergelijken |
| H06 | Kleine testprint | Werkelijke start, materiaalwissel en voltooiing; printen uitsluitend met expliciete opdracht |
| E01 | Parametereffect | Blokbreedte 20→30 mm; gemeten afwijking maximaal 0,05 mm |
| E02 | Parser/validatie | Getal/bool/tekst/keuze correct; unsupported/ongeldige invoer duidelijke melding |
| E03 | Fout en annuleren | Ontbrekende exe, syntaxisfout, lege output, time-out: geen plaatwijziging of oud resultaat |
| E04 | Paden en tekst | Spaties/Unicode/quotes/backslashes werken of worden vooraf duidelijk geweigerd; geen shellinjectie |
| E05 | Plaatimport | Exact één object; huidige plaat; Undo verwijdert het; bestaande objecten/config behouden |
| E06 | Levensduur | Projectwissel/sluiten tijdens render: geen crash, verkeerde import of achtergebleven renderer |
| E07 | Uitgeschakelde editor | Oorspronkelijke import/slicing/opslaan en AMS-flow behouden |
| P01 | Profielanalyse later | Ouders/cycli/ontbrekende sleutels correct; originele profielen bytegelijk |

Voor de eerste lokale editorproef zijn B01–B04 vereist. Voor een printergerichte custom release zijn ook B05–B07 en H01–H06 vereist, met niet-ondersteunde hardwarefuncties expliciet gemotiveerd. Er worden geen live printertests als PASS ingevuld op basis van broncode.
