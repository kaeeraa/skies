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
    pkgs.protobuf
    pkgs.lldb
    pkgs.gdb
    pkgs.mold-wrapped
    pkgs.ccache
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
    statix.enable = true;
  };

  tasks = {
    "skies:build" = {
      exec = "cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=${pkgs.clang.out}/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=${pkgs.clang.out}/bin/clang++ --no-warn-unused-cli -S . -B ./build -G Ninja";
      cwd = "${config.git.root}";
    };
    "skies:run" = {
      exec = "./build/skies";
      before = ["skies:build"];
      cwd = "${config.git.root}";
    };
  };

  process.manager.implementation = "overmind";
  env.CACHIX_AUTH_TOKEN = config.secretspec.secrets.CACHIX_AUTH_TOKEN or "";
}
