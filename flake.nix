{
  description = "C++ wrapper for ACADOS.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";

    acados-overlay.url = "github:adrian-kriegel/acados-nix";
  };

  outputs = { self, nixpkgs, acados-overlay }:
    let
      pkgs = import nixpkgs { 
        system = "x86_64-linux";
        overlays = [ acados-overlay.overlays.default ];
      };
    in {
      packages.x86_64-linux.acados-cpp = pkgs.stdenv.mkDerivation rec {
        pname = "acados-cpp";
        version = "0.1.0";

        src = ./.;

        # Intentionally not adding ACADOS as a dependency. ACADOSCpp is header-only 
        # so nothing links to ACADOS and no build step is required.
        # Users can just use their own ACADOS installation.
        buildInputs = [];

        nativeBuildInputs = [];

        installPhase = ''
          mkdir -p $out/include
          cp -r ${src}/include/* $out/include
          
          mkdir -p $out/lib/cmake/${pname}
          cp ${src}/cmake/* $out/lib/cmake/${pname}
        '';

        meta = with pkgs.lib; {
          description = "C++ wrapper for ACADOS.";
          homepage = "https://github.com/adrian-kriegel/ACADOSCpp";
          license = licenses.mit;
          maintainers = [ maintainers.adrian-kriegel ];
        };
      };
      devShells.x86_64-linux.default = pkgs.mkShell {

        nativeBuildInputs = with pkgs; [
          gcc
          cmake
        ];

        buildInputs = with pkgs; [
          acados
          gtest
        ];

        shellHook = with pkgs; ''
          export ACADOS_DIR=${acados}
        '';
      };
    };
}