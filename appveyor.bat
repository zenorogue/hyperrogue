cd %APPVEYOR_BUILD_FOLDER%
setlocal

if "%TRAVIS_OS_NAME%" == "mingw" (
  set "PATH=C:\%MSYS2_DIR%\%MSYSTEM%\bin;C:\%MSYS2_DIR%\usr\bin;%PATH%"
) else if "%TRAVIS_OS_NAME%" == "windows" (
  call "%MSVC_SETUP_PATH%" %MSVC_SETUP_ARG%
)

if "%TRAVIS_STAGE%" == "before_build" (

  REM Install SDL.
  if "%TRAVIS_OS_NAME%" == "mingw" (
    pacman --noconfirm --noprogressbar -Syu
    pacman --noconfirm --noprogressbar -Sy base-devel
    pacman --noconfirm --noprogressbar -Sy mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_gfx mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_ttf
  ) else if "%TRAVIS_OS_NAME%" == "windows" (
    cd %TEMP%
    powershell Invoke-WebRequest -OutFile SDL.zip -Uri https://www.libsdl.org/release/SDL-devel-1.2.15-VC.zip
    unzip SDL.zip
    mkdir %APPVEYOR_BUILD_FOLDER%\include\SDL
    move SDL-1.2.15\include\*.h %APPVEYOR_BUILD_FOLDER%\include\SDL\
    move SDL-1.2.15\lib\x64\*.* %APPVEYOR_BUILD_FOLDER%\dll\

    powershell Invoke-WebRequest -OutFile SDL_mixer.zip -Uri https://www.libsdl.org/projects/SDL_mixer/release/SDL_mixer-devel-1.2.12-VC.zip
    unzip SDL_mixer.zip
    mkdir %APPVEYOR_BUILD_FOLDER%\include\SDL
    move SDL_mixer-1.2.12\include\*.h %APPVEYOR_BUILD_FOLDER%\include\SDL\
    move SDL_mixer-1.2.12\lib\x64\*.* %APPVEYOR_BUILD_FOLDER%\dll\

    powershell Invoke-WebRequest -OutFile SDL_ttf.zip -Uri https://www.libsdl.org/projects/SDL_ttf/release/SDL_ttf-devel-2.0.11-VC.zip
    unzip SDL_ttf.zip
    mkdir %APPVEYOR_BUILD_FOLDER%\include\SDL
    move SDL_ttf-2.0.11\include\*.h %APPVEYOR_BUILD_FOLDER%\include\SDL\
    move SDL_ttf-2.0.11\lib\x64\*.* %APPVEYOR_BUILD_FOLDER%\dll\
    cd %APPVEYOR_BUILD_FOLDER%
  ) else (
    exit 'Unsupported OS'
  )

  REM Install GLEW if asked for
  if "%HYPERROGUE_USE_GLEW%" == "1" (
    if "%TRAVIS_OS_NAME%" == "mingw" (
      pacman --noconfirm --noprogressbar -Sy mingw-w64-x86_64-glew
    ) else if "%TRAVIS_OS_NAME%" == "windows" (
      cd %TEMP%
      powershell Invoke-WebRequest -OutFile GLEW.zip -Uri https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip
      unzip GLEW.zip
      mkdir %APPVEYOR_BUILD_FOLDER%\include\GL
      move glew-2.1.0\include\GL\* %APPVEYOR_BUILD_FOLDER%\include\GL\
      move glew-2.1.0\lib\Release\x64\* %APPVEYOR_BUILD_FOLDER%\dll\
      move glew-2.1.0\bin\Release\x64\* %APPVEYOR_BUILD_FOLDER%\dll\
      cd %APPVEYOR_BUILD_FOLDER%
    ) else (
      exit 'Unsupported OS'
    )
  )

  REM Install autotools if asked for
  if "%TRAVIS_BUILD_SYSTEM%" == "autotools" (
    if "%TRAVIS_OS_NAME%" == "mingw" (
      pacman --noconfirm --noprogressbar -Sy automake autoconf
    ) else (
      exit 'Unsupported OS'
    )
  )

) else if "%TRAVIS_STAGE%" == "build_script" (

  REM Build hyperrogue.
  if "%TRAVIS_BUILD_SYSTEM%" == "autotools" (
    bash -c "autoreconf -vi"
    bash -c "./configure CXXFLAGS='-Wall -Werror'"
    bash -c "make"
  ) else if "%TRAVIS_BUILD_SYSTEM%" == "Makefile" (
    make -f Makefile.simple
  ) else if "%TRAVIS_BUILD_SYSTEM%" == "msvc" (
    cl -std:c++latest -W2 -WX -EHsc langen.cpp
    .\langen.exe > language-data.cpp
    cl -std:c++latest -W2 -WX -wd4068 -wd4244 -EHsc -bigobj -I include ^
       -DWINDOWS=1 -DCAP_SDLGFX=0 -DCAP_PNG=0 -DCAP_SAVE=0 -DCAP_ROGUEVIZ=1 hyper.cpp ^
       opengl32.lib dll\glew32.lib dll\SDL.lib dll\SDL_mixer.lib dll\SDL_ttf.lib /Fe: hyperrogue.exe
  ) else (
    exit 'Unsupported build system'
  )

) else if "%TRAVIS_STAGE%" == "test_script" (

  set "PATH=%APPVEYOR_BUILD_FOLDER%\dll;%PATH%"
  .\hyperrogue.exe --help

  REM Test "make dist". ("make distcheck" is expected to fail.)
  if "%TRAVIS_BUILD_SYSTEM%" == "autotools" bash -c "make dist"

  REM Test "sudo make install".
  if "%TRAVIS_BUILD_SYSTEM%" == "autotools" bash -c "make install"
)
