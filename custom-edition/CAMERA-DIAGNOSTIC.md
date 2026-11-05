# Cameraonderzoek — 5 november 2026

## Uitkomst

Cloud-livebeeld is **FAIL: 1:403** in zowel onze custom build als de apart bewaarde, ongewijzigde zelfgebouwde upstream-baseline. De officiële geïnstalleerde Bambu Studio toont wel livebeeld. De editorwijzigingen zijn dus niet nodig om deze fout te reproduceren. De exacte afwijzingsreden in de gesloten netwerkplug-in/server is nog niet bewezen; dit onderzoek levert geen camerafix op.

| Vergelijking | Resultaat |
|---|---|
| Bambu Handy | USER PASS |
| Officiële Studio 02.09.01.70 | USER PASS; daarna liveframe en Playing-status door agent waargenomen |
| Ongewijzigde eigen build van release 02.09.01.70 | FAIL: 1:403, ook na camera opnieuw starten |
| Custom build van dezelfde release | FAIL: 1:403 |
| Printerstatus en AMS-slots in custom build | Alleen-lezen gegevens zichtbaar; gedeeltelijk bewijs, geen volledige hardwarekwalificatie |

Vergelijk de builds achtereenvolgens, met dezelfde bestaande accountconfiguratie en netwerkplug-ins. Open Device, start alleen de camera en noteer versie, identiteit van de binary en de uitkomst. Verstuur geen printeropdrachten voor deze vergelijking. De bewaarde baseline blijft ongewijzigd.

## Gecontroleerd

De officiële applicatie heeft een geldige Authenticode-handtekening; beide eigen builds zijn NotSigned. De gebruikte networking- en videosource-plug-ins hebben geldige handtekeningen. De lokale UTC-klok kwam tot op de seconde overeen met de Date-header van een niet-aangemeld verzoek aan Bambu; dat verzoek was geen cameraverzoek. Het publieke certificaat in beide installaties heeft dezelfde X509-thumbprint. Een ander bestandhash bewijst hier geen ander certificaat.

MediaPlayCtrl.cpp, NetworkAgent.cpp en wxMediaCtrl3.cpp zijn ongewijzigd ten opzichte van upstream. De fout ontstaat tijdens het verkrijgen van de camera-URL/toegang, voordat de decoder het beeld laadt. Een codecwijziging is daarmee geen onderbouwde oplossing. Artifacthashes en precieze bewijsgrenzen staan in de [geschoonde registratie](evidence/camera-diagnostic-2026-11-05.json).

## Sterkste aanwijzing, nog geen bewezen oorzaak

Bambu beschrijft in zijn [Security Whitepaper, paragraaf 2.3.4](https://manuals.plus/m/d042a9ec81608fb3217727d4eeabb08ff00755b5b3b2a78669e25179199e8521) dat de Windows/macOS-netwerkplug-in de identiteit van aanroepende software via de handtekening controleert voor beschermde opdrachten. Dit past bij het verschil tussen officiële en eigen builds. Het document bevestigt niet dat precies deze camera-403 door die controle wordt veroorzaakt. Zelf ondertekenen is evenmin bewezen voldoende om door Bambu te worden vertrouwd.

[Upstream issue 11405](https://github.com/bambulab/BambuStudio/issues/11405) bevat vergelijkbare camerafouten bij eigen builds en een verzoek om netwerkdiagnostiek. Ook dit is geen bewijs van onze specifieke afwijzingsreden.

## Vervolg en checkpoint

1. Gebruik de officiële Studio of Handy voor werkend cloud-livebeeld. De officiële Studio is na de vergelijking heropend; de laatste waarneming was een opgeslagen frame met Play-knop, geen bevestiging van continu afspelen na die herstart.
2. Verifieer via Bambu welke ondersteunde integratie of autorisatie voor livebeeld vanuit een eigen build beschikbaar is. Er is nog geen supportbericht verstuurd.
3. Onderzoek eventueel een ondersteunde, door de eigenaar geautoriseerde lokale cameraroute als afzonderlijke proef. Deze is NOT RUN. Volledige LAN Only-modus is geen bewezen vervanging met behoud van Handy; verander tijdens de lopende print geen netwerk- of beveiligingsinstellingen.
4. Voeg voor printerkwalificatie H07 toe: livebeeld starten, stoppen en opnieuw verbinden in de custom build via de gekozen ondersteunde route. Tot die proef slaagt, blijft cameracompatibiliteit een open releaseblokkade.

Geen productcode gewijzigd tijdens dit onderzoek; daarom geen nieuwe build nodig. Geen printer-, firmware-, netwerk- of beveiligingsinstellingen aangepast en geen besturingsopdrachten verzonden. De bestaande print liep door. Geen sleutels of applicatie-identiteiten gekopieerd, geen versleutelde logs ontsleuteld of geüpload. Account-/apparaatgegevens en camera-afbeeldingen blijven buiten deze registratie. Materiaalwissels, drogen, printstart en volledige AMS-praktijk zijn NOT RUN.

## Aanvullend onderzoek: eigen certificaat en Home Assistant

De gebruiker heeft daarna een screenshot aangeleverd van het printvenster met de melding dat de software niet is ondertekend en printfuncties beperkt zijn. Dit is **USER EVIDENCE**, geen door de agent verstuurde printopdracht. De open bron koppelt BAMBU_NETWORK_SIGNED_ERROR (-26) aan deze melding in Jobs/PrintJob.cpp; GUI_App.cpp behandelt daarnaast het plug-inbericht unsigned_studio. De eigenlijke beslissing zit buiten deze GUI. Alleen de melding verwijderen verhelpt de autorisatie niet.

Een lokaal codeondertekeningscertificaat kan onze eigen uitgever identificeren en, na expliciete lokale vertrouwensconfiguratie, door Windows worden vertrouwd. Dat verleent geen Bambu-identiteit of toestemming op printer/cloud. De precieze acceptatieregels van de gesloten plug-in zijn niet openbaar vastgesteld. Er is geen onderbouwde garantie dat een zelfondertekend of betaald codecertificaat deze blokkade opheft. Een eigen-certificaatproef is **NOT RUN**; er zijn geen certificaten aangemaakt, vertrouwensstores gewijzigd of binaries ondertekend.

[Bambu's autorisatie-uitleg](https://blog.bambulab.com/firmware-update-introducing-new-authorization-control-system-2/) noemt zowel externe video als printstart beschermd en verwijst derden naar Bambu Connect. Dat verklaart waarom camera en printen met statusweergave kunnen verschillen. De documentatie dateert uit 2025 en is geen actuele P2S/AMS-acceptatietest. Voor een cloudworkflow met behoud van Handy is een Connect-overdracht van geslicede bestanden een ondersteunde kandidaat; actuele P2S-ondersteuning, AMS-mapping en livebeeld moeten eerst afzonderlijk worden getest. Deze fork bevat nog geen Connect-integratie.

De Home Assistant-vergelijking geeft een concretere lokale camerakandidaat. De [camera-implementatie van ha-bambulab](https://github.com/greghesp/ha-bambulab/blob/main/custom_components/bambu_lab/camera.py) leest het gerapporteerde RTSP-adres; de [eigen URL-helper](https://github.com/greghesp/ha-bambulab/blob/main/custom_components/bambu_lab/pybambu/utils.py) combineert dit met lokaal printeradres en toegangscode voor gebruiker bblp, met standaardpoort 322. Er is geen code overgenomen. [P2S-gebruikers en de beheerder](https://github.com/greghesp/ha-bambulab/issues/1627) bespreken de aparte LAN Only Liveview-schakelaar én instabiliteit/herverbinden. Dat is een werkende route bij anderen, geen bewijs voor de huidige printer/firmware van deze gebruiker.

Onze MediaPlayCtrl.cpp bevat al een lokale route, maar de selectievoorwaarde bij regel 586 kiest deze alleen bij LAN-mode of ontbrekend remoteprotocol. Een cloudprinter met beide protocollen gaat daardoor normaal naar get_camera_url. Dit maakt een expliciete lokale camerakeuze een gerichte kandidaat. De bestaande lokale decoder loopt nog via BambuSource; of die eigen builds accepteert is **NOT RUN**, dus een gegarandeerde fix volgt niet uit deze broninspectie.

Volgende afgebakende proef: controleer alleen de aparte liveview-schakelaar, lokale bereikbaarheid en een geauthenticeerde lokale stream. Houd de algemene LAN Only-modus ongewijzigd; de P2S-handleiding beschrijft dat volledige LAN Only cloud/Handy uitschakelt. Voeg pas na een werkende stream een expliciete lokale camerakeuze toe, met gemaskeerde toegangscode, geen geheime URL in logs en heldere foutmelding. Acceptatie: bewegend beeld gedurende minstens twee minuten, stop/start en herverbinden, Handy apart nog bruikbaar en nul printerbesturingsopdrachten. Alle lokale praktijkproeven staan nu **NOT RUN**.
