# Contributing

**Licence rule: game code is CC0, `platforms/` is MIT.** By opening a pull request you dedicate your changes outside `platforms/` to the public domain under [CC0 1.0](LICENSE), and license your changes inside `platforms/` under the MIT licence.

## Ground rules

- **No game data.** Don't commit assets, binaries, disc images, files from a disc, or disassembly of the game (split or whole), and don't link to them. The build generates its assembly from your own disc and keeps it out of git. See [LEGAL.md](LEGAL.md).
- **No compiler.** The PS2 toolchain is fetched by `scripts/fetch_toolchain.py`. Never commit it.
- **No logos or game artwork**, in the repository or in issues and pull requests.
- **Only your own work, or code under a compatible licence that you name.** Don't copy from unlicensed projects, leaked SDKs or other decompilations without a licence. If you adapt licensed code, say where it comes from in the file's first comment and add it to [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Matching code

- The matching build must stay byte-identical on both discs, Korean (SLPM-64525) and Japanese (SLPS-20137). Build both before you open a pull request; the README has the commands.
- Write C the way a programmer would have written it. Use the original names, types and locals from the Korean disc's debug data (`include/generated/`).
- A function that doesn't match yet goes in `nonmatching/`, not in `src/`. A fake match must be tagged in the source and listed in [FAKEMATCHES.md](FAKEMATCHES.md).

## Port code (`platforms/`)

- Nothing visual or quality-of-life may change the Simulation Core. The lockstep traces must keep matching.
- Keep platform code out of the game C. The game C is shared with the matching build.

## Before you open a pull request

- Run `python3 scripts/audit_repo.py` and make sure it reports no findings.
- Fill in the pull request template.
