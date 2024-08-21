{
  description = "Tilted C++ development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        overlays = [
           # msvc-llvm-nix.overlay
        ];
        pkgs = import nixpkgs {
          inherit system overlays;
          config.allowUnfree = true;  # In case MSVC toolchain is unfree
        };
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc14
            gdb
            libclang
            cmake
            xmake
            gnumake
            stdenv.cc.cc.lib
            #msvc-toolchain
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
            export LD_LIBRARY_PATH=${pkgs.lib.makeLibraryPath [
                pkgs.stdenv.cc.cc
            ]}
            echo "C++ development environment loaded"
          '';
        };
      });
}
