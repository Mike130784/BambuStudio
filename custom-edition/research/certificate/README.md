# Offline certificaatonderzoek

Lees ../../CERTIFICATE-RESEARCH.md voor conclusie en bewijsgrenzen. Dit is geen printerplug-in of printfix.

De eigen C++-bron en CMake/PowerShell-wrappers zijn onderdeel van onze AGPL-projectcode. Externe test- en implementatiebronnen blijven in een aparte checkout onder hun eigen copyright en AGPL-3.0-or-later-licentie volgens de upstreamheader. Er wordt geen externe bron gekopieerd in deze map.

Voor reproductie: clone https://github.com/ClusterM/open-bamboo-networking.git in een eigen map en checkout exact 95d42f8a9793ec77b933dea62f6153ba3541b562. Laat de checkout schoon. Gebruik op Windows Visual Studio 18 2026 met C++-tools en bestaande statische OpenSSL-bibliotheken met dynamische MSVC-runtime. Er zijn geen netwerkdownloads tijdens deze CMake-configuratie.

Voer build-offline.ps1 uit met de verplichte parameters ObnSource (clone), OpenSslRoot (include/lib), VendorRoots (resources/cert/printer.cer uit onze Studio-basis) en BuildRoot (aparte proefbouwmap). Het script controleert de upstreamcommit, schone checkout en CA-bundelhash, bouwt alleen twee testprogramma's en voert ze met een timeout uit. Het installeert niets.

De oorspronkelijke beperkte bouw onder de beheerde procesomgeving bleef hangen. Alleen die geïdentificeerde eigen bouwprocessen zijn gestopt; dezelfde proef is daarna met volledige toegang tot de bestaande bouwtools geslaagd. Dit is een uitvoeringsomgevingsprobleem, geen producttest.

Geen hardware- of cloudacceptatieclaim: een geslaagde cryptografische proef geeft onze uitgever geen autorisatie op de printer.
