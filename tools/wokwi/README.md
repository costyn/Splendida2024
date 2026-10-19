# Simulating the lamp with Wokwi

[Wokwi](https://wokwi.com) simulates the ESP32 and the 256 LEDs (Wokwi's own `wokwi-splendida` part) so the firmware can be
booted and checked without hardware. Everything runs in Wokwi's cloud, so a free account and an API token are needed for
anything beyond linting. This was set up without a token, so **the simulation itself has not been run yet**; see
"What is verified" below.

Files: `../../wokwi.toml` (firmware paths), `../../diagram.json` (ESP32 DevKit-C v4 + `wokwi-splendida`, data on GPIO 22,
serial monitor wired), `smoke.sh`, `scenarios/patterns.test.yaml`, `../../.github/workflows/wokwi.yml`.

## 1. Get a token (once)

1. Sign in at <https://wokwi.com> (free account).
2. Open <https://wokwi.com/dashboard/ci> and create a token. It starts with `wok_`.
3. Free plan limits: 50 simulated minutes per rolling 30 days, 5 minutes wall clock per run. A CI run here uses about 25 s.

## 2. Run locally

```
curl -L https://wokwi.com/ci/install.sh | sh        # installs ~/bin/wokwi-cli (Linux, macOS)
pio run -e m5stack_atommatrix                       # Wokwi does not compile, it needs the build output
export WOKWI_CLI_TOKEN=wok_...
tools/wokwi/smoke.sh                                # boot test, stops at the first Pattern line
tools/wokwi/smoke.sh --patterns                     # also waits for the first palette change (~19 s simulated)
tools/wokwi/smoke.sh --screenshot                   # additionally tries a PNG of the LEDs (may not work, see below)
tools/wokwi/smoke.sh --lint                         # diagram.json check only: offline, no token
```

Output goes to `.pio/wokwi/` (`serial.log`, `screenshot.png`). Without a token `smoke.sh` lints the diagram, prints how to get
a token and exits with status 2. Without the script: `wokwi-cli --timeout 10000 --expect-text "printPatternAndPalette(): Pattern:" .`

What the test checks: `setup()` ends by calling `changePalette()`, which prints
`<time>: printPatternAndPalette(): Pattern: <name> \tPalette: <name>`. Seeing that line means bootloader, Arduino core, FastLED
init (including the RMT output on GPIO 22) and the task scheduler all ran. `--fail-text "Guru Meditation"` catches panics. The
`--patterns` scenario additionally needs a second such line, which the palette task prints 19 s later.

Useful `wokwi-cli` options: `--timeout <ms>` (simulated time; exit code 42 on timeout), `--expect-text` (the run ends as soon
as it is seen), `--fail-text` (one string only), `--serial-log-file`, `--scenario file.yaml` (steps: `wait-serial`, `delay`,
`write-serial`, `set-control`, `expect-pin`, `take-screenshot`; alpha), `--interactive` (stdin to serial).
Not useful here: heap monitoring (ESP-IDF only), SD card, VCD (needs a logic analyzer part in the diagram).

## 3. Run in GitHub Actions

`.github/workflows/wokwi.yml` runs on every push and manually (Actions tab > Wokwi > Run workflow). It always builds the
firmware and uploads `firmware-merged.bin`; the simulation steps only run when the secret exists:

GitHub repo > Settings > Secrets and variables > Actions > New repository secret > name `WOKWI_CLI_TOKEN`, value `wok_...`.

Without it the workflow passes and prints a notice. The artifact `wokwi` holds `serial.log`, `screenshot.png` (if it worked)
and the merged firmware. The action is `wokwi/wokwi-ci-action@v1` (inputs: `token`, `path`, `timeout` in ms, `expect_text`,
`fail_text`, `scenario`, `serial_log_file`, `args`). `expect_text` ends the run at the first match, so it cannot be combined
with a longer scenario; the workflow uses the scenario alone.

## 4. On wokwi.com from a phone

wokwi.com is a web app and loads in a phone browser, but its editor is built for a desktop and is cramped on a phone. The
simulation view works; I have not tried the steps below on a phone.

The lamp cannot be compiled on wokwi.com (PlatformIO project with pinned libraries, a git-only library and sub-folders), so
upload the built firmware:

1. Get a **merged** image (bootloader + partitions + app, which Wokwi recommends for uploads): the `wokwi` artifact of a
   workflow run (GitHub > Actions > the run > Artifacts; it is a zip), or locally `pio run -e m5stack_atommatrix -t mergebin`
   (`.pio/build/m5stack_atommatrix/firmware-merged.bin`).
2. Open the "ESP32 custom application" template: <https://wokwi.com/projects/305457271083631168>.
3. Replace its `diagram.json` tab with this repo's `diagram.json` (GitHub raw view, select all, copy, paste).
4. In the code editor press **F1** and choose "Upload Firmware and Start Simulation...", pick the `.bin`. This is the only
   documented route, and F1 needs a keyboard: on a phone use "Desktop site" and a Bluetooth keyboard, or do this step on a computer.
   Whether the on-screen keyboard can open the command palette is unknown.

## What is verified

Verified here (no token): `wokwi-cli` 0.29.0 installs and runs; `diagram.json` and `wokwi.toml` parse and pass
`wokwi-cli lint` (two "undocumented part type" infos for the DevKit and `wokwi-splendida`, which are fine); the CLI reaches
the Wokwi server and then stops with `API Error: Unauthorized` for a fake token, and without any token with
`Error: Missing WOKWI_CLI_TOKEN environment variable.` (exit 1); the expected serial string exists in the built firmware.

Not verified (needs a token): that the firmware boots and prints in the simulator, how fast it simulates, and the screenshot.

Notes:

- **Screenshots**: `--screenshot-part` / `--screenshot-time` and the `take-screenshot` scenario step call Wokwi's
  `framebuffer:read`, which is documented for displays (the CLI's own example is an LCD). `wokwi-splendida` is an undocumented
  part; it may well have no framebuffer. The workflow and `smoke.sh --screenshot` try it without failing the build. To prove LED
  output without a screenshot, add a `wokwi-logic-analyzer` on GPIO 22 and use `--vcd-file`.
- **`serialMonitor`**: not needed. The CLI listens to the serial port through the simulation API. The `$serialMonitor`
  connections in `diagram.json` are what the web editor and VS Code use to show the serial monitor.
- **`[[net]]` / `rfc2217ServerPort`**: not needed. Only for WiFi port forwarding and for attaching a serial monitor in VS Code.
- **App-only firmware**: `firmware.bin` is fine here; Wokwi supplies its own bootloader and default partition table. Use the
  merged image if partitions ever change.
- **Encoder**: there is no Wokwi part for the DuPPa I2C Encoder V2. The firmware tolerates its absence (INT on GPIO 19 has a
  pull-up and is never low; I2C writes to 0x28 are NACKed), so the simulation shows the idle pattern rotation only (patterns
  change every 123 s, palettes every 19 s). Brightness, speed and playlist control cannot be exercised.
- **Encoder as a custom chip**: possible in principle. Wokwi custom chips are C compiled to WASM (`wokwi-cli chip compile`),
  loaded with `[[chip]]` in `wokwi.toml`, with an I2C device API and GPIO pins for INT. It would need the register map of the
  DuPPa chip (config, status, 32-bit counter, min/max/step, RGB, id/version; see `i2cEncoderLibV2.h`) and the interrupt/clear-on-read
  behaviour, about 200 lines of C. Inputs would be range sliders only (no knob or button), driven by `set-control` in a scenario.
  Estimated half a day to a day including debugging, and not testable without a token, so it is not implemented.
