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
