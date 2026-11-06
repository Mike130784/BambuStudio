# Roadmap en concrete acceptatie

De eerste P1c-stap is afgerond: numerieke Customizer-koppen zonder schema, twee eigen modellen en stapcontrole. Zie [scope](CUSTOMIZER.md) en [checkpoint](CHECKPOINT.md). Dependencies, andere parametertypes, opgeslagen parameterwaarden en brede modelcompatibiliteit blijven open.

## Toegevoegd op verzoek: eenvoudige modelbewerking (toekomst)

Doel: bruikbare basisbewerkingen in de slicer, met een overzichtelijke bediening zoals bij bekende 3D-programma's. Dit is toekomstplanning; er wordt geen volledige Blender-, Maya- of 3ds Max-omgeving gebouwd.

| Stap | Functies | Voorwaarde |
|---|---|---|
| M1 | Verplaatsen, schalen, draaien, spiegelen, uitlijnen en snijden | Bestaande Bambu-tools hergebruiken en duidelijk bereikbaar maken |
| M2 | Eenvoudige vormen toevoegen, gaten/uitsparingen en samenvoegen | Bestaande mesh-booleans benutten waar geschikt; afmetingen numeriek instelbaar |
| M3 | Vlakken of vlakke groepen selecteren, verplaatsen en eenvoudig extruderen | Eerst beperkt tot geschikte, gevalideerde gesloten meshes; resultaat vooraf tonen |
| M4 | Randen afschuinen/afronden | Alleen ondersteunde randen/topologie; ongeschikte modellen duidelijk weigeren |

Werk altijd op een bewerkbare kopie met Undo/Redo en een expliciete toepasactie. Bewaar oorspronkelijke geometrie en SCAD-bron. Vrije meshbewerkingen worden niet automatisch terugvertaald naar SCAD-parameters. Test geslotenheid, oriëntatie, afmetingen, nieuwe zelfdoorsnijdingen, slicing en 3MF opslaan/heropenen vóór vrijgave. Deze functies komen na de huidige lokale editor en zijn nog niet geïmplementeerd.

De huidige UI-stap omvat een Bambu-stijlvenster, lokaal onthouden renderer/modelpad en dezelfde ingang via Bestand en rechtermuisknopmenu. Een contextmenu maakt een gewone STL nog niet parametrisch.

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

Cameraonderzoek 5 november: status en AMS-slots zijn alleen-lezen zichtbaar; H01/H02 zijn daarmee nog geen volledige hardwarekwalificatie. H07 is voor de expliciete lokale RTSPS-route op P2S 01.03.00.00 praktisch PASS, inclusief stop/start en herstart. [Bewijs](evidence/local-camera-acceptance-2026-11-05.json). Cloudcamera blijft FAIL; rechtstreeks printen is afzonderlijk geblokkeerd. De officiële Connect-overdracht en AMS-toewijzing in het voorbereidingsscherm zijn inmiddels PASS; Send/printstart en fysieke wissel blijven NOT RUN. Zie [Connect](CONNECT-HANDOFF.md). H03-H06 en volledige printerkwalificatie blijven open.

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
| H07 | Livebeeld | Starten, stoppen en opnieuw verbinden via een ondersteunde route in de custom build; lokale route P2S 01.03.00.00 PASS, oorspronkelijke cloudroute FAIL: 1:403 |
| H06 | Kleine testprint | Werkelijke start, materiaalwissel en voltooiing; printen uitsluitend met expliciete opdracht |
| E01 | Parametereffect | Blokbreedte 20→30 mm; gemeten afwijking maximaal 0,05 mm |
| E02 | Parser/validatie | Getal/bool/tekst/keuze correct; unsupported/ongeldige invoer duidelijke melding |
| E03 | Fout en annuleren | Ontbrekende exe, syntaxisfout, lege output, time-out: geen plaatwijziging of oud resultaat |
| E04 | Paden en tekst | Spaties/Unicode/quotes/backslashes werken of worden vooraf duidelijk geweigerd; geen shellinjectie |
| E05 | Plaatimport | Exact één object; huidige plaat; Undo verwijdert het; bestaande objecten/config behouden |
| E06 | Levensduur | Projectwissel/sluiten tijdens render: geen crash, verkeerde import of achtergebleven renderer |
| E07 | Uitgeschakelde editor | Oorspronkelijke import/slicing/opslaan en AMS-flow behouden |
| P01 | Profielanalyse later | Ouders/cycli/ontbrekende sleutels correct; originele profielen bytegelijk |

Voor de eerste lokale editorproef zijn B01–B04 vereist. Voor een printergerichte custom release zijn ook B05–B07 en H01–H07 vereist, met niet-ondersteunde hardwarefuncties expliciet gemotiveerd. Er worden geen live printertests als PASS ingevuld op basis van broncode.

## 6 november: eigenaarstest en onderzoek hybride printen

Directe LAN-printdispatch en Automatic-camera USER PASS na LAN Only + Developer Mode. Handy hierdoor niet beschikbaar. Eigen certificaat/software-ondertekening offline onderzocht (2/2 programma's PASS); geen eigenaar-rootregistratie gevonden en geen hybride printfix opgeleverd. Zie CERTIFICATE-RESEARCH.md. Printvoltooiing, fysieke AMS-wissels en hardwareacceptatie van eigen certificaten blijven afzonderlijk onbewezen.
