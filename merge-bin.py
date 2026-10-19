#!/usr/bin/python3

# Adds PlatformIO post-processing to merge all the ESP flash images into a single image.

import os

Import("env", "projenv")

board_config = env.BoardConfig()
firmware_bin = "${BUILD_DIR}/${PROGNAME}.bin"
merged_bin = os.environ.get("MERGED_BIN_PATH", "${BUILD_DIR}/${PROGNAME}-merged.bin")


def merge_bin_action(source, target, env):
    flash_images = [
        *env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])),
        "$ESP32_APP_OFFSET",
        source[0].get_abspath(),
    ]
    merge_cmd = " ".join(
        [
            # $OBJCOPY is the esptool executable (esptool 5+ is a standalone
            # binary, not a Python script, so it must not be run via $PYTHONEXE).
            '"$OBJCOPY"',
            "--chip",
            board_config.get("build.mcu", "esp32"),
            "merge-bin",
            "-o",
            merged_bin,
            "--flash-mode",
            board_config.get("build.flash_mode", "dio"),
            "--flash-freq",
            "${__get_board_f_flash(__env__)}",
            "--flash-size",
            board_config.get("upload.flash_size", "4MB"),
            *flash_images,
        ]
    )
    env.Execute(merge_cmd)


env.AddCustomTarget(
    name="mergebin",
    dependencies=firmware_bin,
    actions=merge_bin_action,
    title="Merge binary",
    description="Build combined image",
    always_build=True,
)