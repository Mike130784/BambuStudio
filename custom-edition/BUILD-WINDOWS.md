# Windows-build van de ontwikkelbranch

De build is lokaal uitgevoerd met Visual Studio Community 2026 (18.10), MSVC 14.51, SDK 10.0.26100.0 en de gebundelde CMake 4.3.1. De bij deze bron horende Windows-CI gebruikt VS2026. Een andere toolchain of dependencyversie vraagt een nieuwe verificatie; de oudere upstream-wiki is niet de enige bouwspecificatie voor deze revisie.

## Benodigd

- Visual Studio 2026 met x64 C++-tools en Windows SDK; open de bijbehorende Developer PowerShell.
- CMake en Git, Windows Perl en een Windows pkg-config-programma. Geef de werkelijke installatiepaden op.
- Een korte nieuwe bouwmap met voldoende vrije ruimte, bijvoorbeeld `D:\bsc-custom`. Hergebruik geen CMake-cache van een andere bronmap/revisie.
- OpenSCAD afzonderlijk voor de editor. De geteste officiële portable versie is 2021.01; kies voor deze proef `openscad.com` in het editorvenster.

Lokaal gebruikt: [Strawberry Perl 5.42.3.1 portable](https://github.com/StrawberryPerl/Perl-Dist-Strawberry/releases/tag/SP_54231_64bit) en [pkgconf 3.0.10](https://github.com/pkgconf/pkgconf/releases/tag/pkgconf-3.0.10). SHA256 van de gebruikte downloads:

- Perl ZIP: `6a081a811781c30aca51dbc036afd93092af91e3297901f02c17043795a10690`.
- pkgconf x64 MSI: `28cc9e44473a3ef277260f3dac20f58e2a667760393bd47dc309b6426e9bd727` (lokaal administratief uitgepakt, geen algemene installatie).
- [OpenSCAD 2021.01 x64 ZIP](https://github.com/openscad/openscad/releases/tag/openscad-2021.01): `fb0caabf5bbc89f8f2f80c10b79ae64d697aaff6efd58b2756f5d6270edb7ba7`, gecontroleerd tegen de officiële checksum.

## Bouwrecept

Onderstaand recept correspondeert met de geslaagde lokale CMake-stappen. Stel de padvariabelen in voor jouw machine. De nieuwe GitHub-clone op een andere machine is nog niet afzonderlijk gebouwd.

```powershell
$sourceRoot = (Get-Location).Path # repositoryroot
$buildRoot = 'D:\bsc-custom'
$cmake = 'C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
$perlBin = 'C:\tools\strawberry\perl\bin' # aanpassen
$pkgConfig = 'C:\tools\pkgconf\pkg-config.exe' # aanpassen
$sdkInclude = 'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0'
$env:Path = "$perlBin;$env:Path"
$env:PKG_CONFIG_EXECUTABLE = $pkgConfig
$env:LC_ALL = 'C'
$env:LANG = 'C'
$env:npm_config_virtual_store_dir = "$buildRoot\web-virtual-store"
New-Item -ItemType Directory -Force -Path $buildRoot | Out-Null
Push-Location $buildRoot # dependencypatches vanaf buiten de bronrepository
try {
    & $cmake -S "$sourceRoot\deps" -B "$buildRoot\deps" -G 'Visual Studio 18 2026' -A x64 "-DDESTDIR:PATH=$buildRoot\deps\prefix" -DCMAKE_BUILD_TYPE=Release -DDEP_DEBUG=OFF
    if ($LASTEXITCODE) { throw 'Dependencyconfiguratie mislukt' }
    & $cmake --build "$buildRoot\deps" --config Release --parallel 1
    if ($LASTEXITCODE) { throw 'Dependencybuild mislukt' }
    & $cmake -S $sourceRoot -B "$buildRoot\app" -G 'Visual Studio 18 2026' -A x64 -DBBL_RELEASE_TO_PUBLIC=1 -DBBL_INTERNAL_TESTING=0 "-DCMAKE_PREFIX_PATH=$buildRoot\deps\prefix\usr\local" "-DCMAKE_INSTALL_PREFIX=$buildRoot\install" -DCMAKE_BUILD_TYPE=Release "-DWIN10SDK_PATH=$sdkInclude" "-DPKG_CONFIG_EXECUTABLE=$pkgConfig"
    if ($LASTEXITCODE) { throw 'Appconfiguratie mislukt' }
    & $cmake --build "$buildRoot\app" --target install --config Release --parallel 2
    if ($LASTEXITCODE) { throw 'Appbuild mislukt' }
} finally { Pop-Location }
& "$buildRoot\install\bambu-studio.exe" --datadir "$buildRoot\test-data"
```

Controleer uitvoer/logs en hashes na elke stap. Voor een verse testdatamap kan de oorspronkelijke configuratiewizard verschijnen; de gebruiker handelt account-, privacy- en licentievragen zelf af. Gebruik niet de dagelijkse Bambu-configuratie. `--no-single-instance` is in de huidige CLI-configuratie uitgeschakeld; vertrouw daar niet op. De eerste start kan vertraagd zijn: controleer het werkelijke proces/venster voordat hetzelfde model nogmaals wordt geïmporteerd.

De korte bouwmap verhielp de aangetroffen OCCT C1083-fout (objectpad 260→165 tekens). De procesinstelling voor pnpm verkortte het Vite-packagepad van 274 naar 119 tekens zonder bron-/lockfilewijziging. Upstream-waarschuwingen zoals C4018/C4996 en LNK4098 zijn bewaard, niet stil onderdrukt. Pnpm-installatiescripts voor oxide/esbuild zijn niet algemeen goedgekeurd; de daadwerkelijke webbuild slaagde.

Niet elke upstream-dependency is onveranderlijk vastgezet. De lokale wxWidgets-commit was `abb542694ef4534a50e65c5b877b2774d02952ae`; een verse build kan een andere `master` ophalen. De volledige lokale inventaris en caches blijven bij het checkpoint bewaard. Een geslaagde nieuwe build moet zijn eigen dependencyherkomst vastleggen.

## Native rendererproef

De testbron gebruikt dezelfde `WindowsRenderJob.hpp` als de editor. Vanuit Developer PowerShell, met een verse uitvoermap en een parameterbestand volgens het schema hieronder:

```powershell
cl /nologo /std:c++17 /EHsc /DNOMINMAX custom-edition\tests\test-render-job.cpp /Fe:D:\bsc-custom\test-render-job.exe /Fo:D:\bsc-custom\test-render-job.obj
# Alle paden absoluut; laatste argument moet een nog niet bestaande map zijn.
D:\bsc-custom\test-render-job.exe C:\tools\openscad\openscad.com C:\src\BambuStudio\custom-edition\samples\parametric-block.scad D:\bsc-custom\parameters.json 'D:\bsc-custom\proef ü 30'
```

`parameters.json`: `{"fileFormatVersion":"1","parameterSets":{"custom":{"width":"30","depth":"10","height":"5"}}}`. De test controleert echte renderuitvoer, ontbrekende renderer, foutieve/lege geometrie, argumentquoting en stoppen van eigen ouder-/kindprocessen. Meet daarnaast de resulterende STL: 30×10×5 mm, maximaal 0,05 mm afwijking. Dit vervangt geen GUI- of printertest.
