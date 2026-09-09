{
  description = "Skyrim Together Reborn linux server build environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            # gcc 15 rejects sol2 v3.3.0, which xmake.lua pins
            gcc14
            gdb
            xmake

            # xmake builds most of its dependencies from source
            cmake
            gnumake
            ninja
            pkg-config
            perl
            python3

            # BuildInfo.h is generated from the git history
            git

            curl
            unzip
          ];

          shellHook = ''
            # Unset environment variables, required for xmake to find
            # the linker/compiler that we provide here
            unset CC
            unset CXX
            unset LD
            unset AR
            unset AS
            unset RANLIB
            unset STRIP
            unset CFLAGS
            unset CXXFLAGS
            unset LDFLAGS

            echo "Linux server build environment loaded, run: xmake config -m releasedbg && xmake"
          '';
        };
      });
    };
}
