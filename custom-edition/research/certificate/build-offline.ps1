[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$ObnSource,
    [Parameter(Mandatory=$true)][string]$OpenSslRoot,
    [Parameter(Mandatory=$true)][string]$VendorRoots,
    [Parameter(Mandatory=$true)][string]$BuildRoot
)
$ErrorActionPreference = 'Stop'
$expectedCommit = '95d42f8a9793ec77b933dea62f6153ba3541b562'
$head = & git -C $ObnSource rev-parse HEAD
if ($LASTEXITCODE -ne 0 -or $head -ne $expectedCommit) { throw 'Unexpected upstream commit' }
$status = & git -C $ObnSource status --porcelain
if ($LASTEXITCODE -ne 0 -or $status) { throw 'Upstream research checkout must be clean' }
if ((Get-FileHash -LiteralPath $VendorRoots -Algorithm SHA256).Hash -ne
    '36F2BCEE347EC7ADCE719B5FD350099591A4D3D0EC4E039C7019890D78E152A0') {
    throw 'Unexpected public CA bundle; review a changed bundle before testing'
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vsRoot) { throw 'Visual Studio C++ tools required' }
$cmakeBin = Join-Path $vsRoot 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin'
$cmake = Join-Path $cmakeBin 'cmake.exe'
$ctest = Join-Path $cmakeBin 'ctest.exe'
& $cmake -S $PSScriptRoot -B $BuildRoot -G 'Visual Studio 18 2026' -A x64 `
    "-DOBN_SOURCE=$([IO.Path]::GetFullPath($ObnSource))" `
    "-DOPENSSL_ROOT_DIR=$([IO.Path]::GetFullPath($OpenSslRoot))" `
    "-DVENDOR_ROOTS=$([IO.Path]::GetFullPath($VendorRoots))"
if ($LASTEXITCODE -ne 0) { throw 'Research configure failed' }
& $cmake --build $BuildRoot --config Release --parallel 2 -- /nodeReuse:false
if ($LASTEXITCODE -ne 0) { throw 'Research build failed' }
& $ctest --test-dir $BuildRoot -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) { throw 'Offline research tests failed' }
