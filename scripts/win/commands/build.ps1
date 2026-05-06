# Build the main project.

$buildType = cmake_build_type
$nproc = portable_nproc

Write-Host "==> Building main project ($script:CONFIG)"
cmake -S $script:REPO_ROOT -B $script:MAIN_BUILD_DIR `
    -G Ninja `
    "-DCMAKE_BUILD_TYPE=$buildType" `
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

cmake --build $script:MAIN_BUILD_DIR -j $nproc
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
