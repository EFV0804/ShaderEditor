
set buildDir=%~dp0..\assets\shaders\compiled
set shaderDir=%builddir%\assets\shaders
if not exist %buildDir% (
    mkdir %buildDir%
)
cd %~dp0..\assets\shaders
for %%i in (*.vert *.frag *.comp *.tesc *.tese *.geom) do "C:\VulkanSDK\1.3.224.1\Bin\glslangValidator.exe" -V "%%~i" -o "%buildDir%\%%~i.spv"
