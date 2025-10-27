{
  pkgs,
  config,
  ...
}: {
  # https://devenv.sh/packages/
  packages = [
    pkgs.git
    pkgs.ninja
    pkgs.boost
    pkgs.secretspec
    pkgs.openssl
  ];

  # https://devenv.sh/languages/
  languages = {
    cplusplus.enable = true;
    nix.enable = true;
  };

  cachix = {
    enable = true;

    pull = ["kaeeraa"];
    push = "kaeeraa";
  };

  git-hooks.hooks = {
    shellcheck.enable = true;
    mdsh.enable = true;
    alejandra.enable = true;
    cmake-format.enable = true;
    deadnix.enable = true;
    markdownlint.enable = true;
    statix.enable = true;
  };

  env.CACHIX_AUTH_TOKEN = config.secretspec.secrets.CACHIX_AUTH_TOKEN or "";
}
