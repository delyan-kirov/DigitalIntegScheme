{
  description = "DigitalIntegScheme build environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    pkgs = import nixpkgs { system = "x86_64-linux"; };
  in {
    devShells.x86_64-linux.default = with pkgs; mkShell {
      buildInputs = [
        pkgs.bear # optional
        pkgs.gcc # needs to build cpp 20
        pkgs.gnumake
      ];
    };
  };
}
