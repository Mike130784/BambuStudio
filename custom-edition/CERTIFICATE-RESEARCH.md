# Eigen certificaat en direct printen met behoud van Handy

Onderzoek 6 november 2026. Vraag: kunnen we Open Bamboo Networking aanpassen zodat onze Windows-fork zonder externe Connect-app print, terwijl Bambu Handy blijft werken?

## Uitkomst

De open-source ondertekeningscode is daadwerkelijk gebouwd en getest met eigen sleutels. Correct ondertekenen is uitvoerbaar. Er is echter geen aangetoonde manier gevonden om een eigen uitgever op de normale P2S-firmware als vertrouwde autoriteit toe te voegen met behoud van cloudverbinding. Er is dus GEEN werkende hybride printfix opgeleverd. De juiste status is: softwareproeven PASS, printeracceptatie eigen certificaat NOT RUN, gevraagde combinatie ONOPGELOST.

Mijn eerdere suggestie dat app_cert_install misschien een algemene registratie voor eigen certificaten was, was te optimistisch. De naam van dat commando is daarvoor geen bewijs.

## Doorslaggevende bronbevinding

De netwerkprojectdocumentatie beschrijft dat een onvolledige applicatieketen al een SUCCESS bij installatie kan opleveren, waarna opdrachten alsnog worden afgewezen omdat de firmware geen keten naar haar ingebouwde BBL CA kan opbouwen. Het teruggeven van SUCCESS is dus geen bewijs van printautorisatie. Dit is een hardwarebevinding van de projectauteur, geen proef op Mike's P2S-firmware 01.03.00.00. [Vastgepinde protocolbeschrijving, regel 194](https://github.com/ClusterM/open-bamboo-networking/blob/95d42f8a9793ec77b933dea62f6153ba3541b562/research/08.04-lan.md#L194).

## Wat is onderzocht

- Open Bamboo Networking, vastgepind op `95d42f8a9793ec77b933dea62f6153ba3541b562`, commitdatum 3 november 2026. Onderzocht: src/agent.cpp, signing.cpp, cert_store.cpp, tests/signing_test.cpp en research/08.04, 10.01, 10.02, 10.04, 13.01 en 13.02. Checkout schoon gehouden; geen upstreamprogramma voor credentialextractie uitgevoerd.
- Onafhankelijke protocolbeschrijving OpenBambuAPI, commit `cc383a2c96576a9f53391c879acfb8dca5c534e4`. Dit is gemeenschapsonderzoek, geen officiële garantie en geen eigen firmwarebron.
- De open GUI/wrapper van onze Studio-fork op `62d2a65faa99667fec1037b7196617b6130664f5`. Beslissing over -26 komt vanuit de gesloten netwerklaag.
- Publieke documentatie en repository gezocht op rootinstallatie/eigen CA/owner-enrollment. Geen gedocumenteerd eigenaarspad aangetroffen. Afwezigheid in deze bronnen bewijst niet dat iedere firmwareversie of toekomstige Bambu-integratie zo'n pad onmogelijk maakt. Firmwarecode is hier niet beschikbaar en niet geanalyseerd.

## Drie controles die niet hetzelfde zijn

1. **Windows-codehandtekening.** Identificeert de uitgever van EXE/DLL voor Windows en eventueel een afnemende plug-in. Een eigen certificaat kan dit lokaal verzorgen, maar verandert de firmware niet.
2. **Applicatiecertificaat voor printeropdrachten.** Koppelt een publieke sleutel aan een vertrouwde uitgever. Een client kan zelf sleutelparen en certificaten genereren; de ontvangende printer bepaalt welke uitgevers hij vertrouwt.
3. **Cloudautorisatie.** De cloud stelt eigen eisen. Werkt een opdracht lokaal, dan is daarmee geen cloudtoegang bewezen, en omgekeerd. Handy hangt bovendien af van de cloudverbinding van de printer.

De huidige open-source cloudroute verwacht reeds bestaande Bambu-slicercredentials. Dat maakt dit geen zelfservice-uitgifte van onze eigen identiteit. Een gewone gebruikerslogin is niet hetzelfde als zo'n applicatiecredential. [Projectbeschrijving](https://github.com/ClusterM/open-bamboo-networking#two-ways-to-run).

Een onafhankelijk geobserveerde Windows-plug-incontrole vergelijkt de signeridentiteit van host EXE en Studio DLL. Alleen dezelfde naam in een eigen certificaat zetten levert die publieke sleutel niet op. Dit is beschreven voor plug-in 02.07; exacte interne werking van onze 02.09-plug-in is niet opnieuw omgekeerd of bewezen. [OpenBambuAPI](https://github.com/Doridian/OpenBambuAPI/blob/cc383a2c96576a9f53391c879acfb8dca5c534e4/client-plugin-internals.md#note-for-custom--forked-builds-interoperability).

## Wat daadwerkelijk gebouwd en uitgevoerd is

In een afzonderlijke onderzoeksmap, met MSVC x64 en de reeds aanwezige OpenSSL 3.1.2-bibliotheken:

- **Upstream signing_test:** 21 subproeven in één native testprogramma; PASS. Deze maken eigen tijdelijke RSA-testsleutels, controleren ondertekening, gewijzigde berichtvormen en de scheiding van versleuteling/ondertekening. Negatieve fixtures melden bewust ontbrekende test-devicekeys; het programma eindigt met signing_test: ok.
- **Eigen certificate_trust_lab:** 5 controles; PASS. Een eigen leafcertificaat heeft een geldige handtekening, wordt vertrouwd met de eigen root als trust anchor en afgewezen met een andere root of met Studio's publieke printer-CA-bundel. De eigen root als gewone keten meesturen verandert dit niet.
- **CTest:** 2/2 programma's PASS. Geen socketfuncties in de eigen proef; geen Agent, MQTT-client of cloudclient gebouwd/geladen door deze CMake-targets.

Cruciale beperking: de eigen proef gebruikt OpenSSL en de publieke CA-bundel uit Studio. Dit modelleert normale X.509-ketenvalidatie; het is NIET de firmwareverifier, NIET haar exacte truststore en GEEN hardwarebewijs. De ondertekeningsproef bewijst evenmin dat de P2S de eigen testidentiteit accepteert.

De bron, bouwinstructie en geschoonde resultaten staan in research/certificate en evidence/certificate-research-2026-11-06.json. Volledige alternatieve netwerk-DLL, DirectShow-registratie en installatie zijn niet uitgevoerd. Bestaande slicerbinaries, profielplug-ins, Windows-vertrouwen en printerinstellingen zijn niet gewijzigd. Geen printopdracht verzonden. Testcertificaten van de eigen proef blijven uitsluitend in geheugen; upstream tijdelijke sleutelbestanden worden door zijn test opgeruimd. Er zijn geen Bambu-private sleutels verkregen of gebruikt.

## Betekenis voor de gewenste oplossing

| Richting | Stand |
|---|---|
| Eigen Windows-certificaat | Kan lokale herkomst tonen; geen aangetoonde oplossing voor printer/cloudautorisatie |
| Eigen signing-code | Gebouwd en getest; alleen voldoende wanneer de ontvanger de uitgever accepteert |
| Eigen root uploaden als applicatieketen | Maakt die root volgens normale ketenvalidatie niet vertrouwd; geen gedocumenteerde root-enrollment gevonden |
| Alleen de open-source plug-in wijzigen | Verandert printer/cloud-verificatie niet |
| LAN Only + Developer Mode | Direct starten en automatische camera door gebruiker bevestigd; Handy ontbreekt in die modus |
| Cloudmodus + bestaande Connect-overdracht | Eerder printstart door gebruiker bevestigd; huidige afzonderlijke app blijft nodig |
| Bambu autoriseert onze eigen identiteit/native integratie | Concrete externe vervolgvraag; beschikbaarheid, voorwaarden en kosten onbekend |

Een eventuele firmwarefunctie voor een door de eigenaar vertrouwde root zou verschil maken. Er is nu geen onderbouwde firmwarewijziging die we daarvoor veilig kunnen bouwen; het oplossen van die externe afhankelijkheid is niet hetzelfde als een plug-in aanpassen.

## Concrete vervolgvraag aan Bambu

Bambu verwijst integratievragen naar devpartner@bambulab.com. Vraag of zij voor persoonlijk gebruik een eigen applicatiecertificaat of native SDK-integratie aanbieden die cloud/Handy behoudt en geen afzonderlijke Connect-app nodig heeft; of de P2S 01.03.00.00 eigenaar-CA-enrollment kent; en welke certificaatuitgifte, compatibiliteit en voorwaarden gelden. Er is geen bericht verstuurd. [Officiële integratietoelichting](https://blog.bambulab.com/updates-and-third-party-integration-with-bambu-connect/).

## Praktisch bewijs van de gebruiker

6 november: gebruikersfoto bevestigt LAN Only, lokale liveweergave en Developer Mode aan. Gebruiker bevestigt opnieuw verbonden printer, livebeeld in Automatic (original) en direct printen. Screenshot toont aangenomen taak in Homing toolhead bij 0/400 lagen. Dat kwalificeert als USER PASS voor dispatch/voorbereiding, nog niet als voltooid printresultaat of fysieke AMS-wissel. Geen codes, adressen of gebruikersafbeeldingen in de gepubliceerde registratie.
