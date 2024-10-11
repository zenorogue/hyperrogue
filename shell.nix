{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    nativeBuildInputs = with pkgs.buildPackages; [
      gnumake
      libgcc
      haskellPackages.SDL
      haskellPackages.SDL-ttf
      haskellPackages.SDL-gfx
      haskellPackages.SDL-mixer
      glew
    ];
}
