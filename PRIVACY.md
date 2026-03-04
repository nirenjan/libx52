# Privacy

This document describes how the libx52 project handles data, for compliance with
privacy laws such as the GDPR and CCPA.

## Summary

- **No automatic collection**: The software does not transmit any data to the
  project or third parties. No telemetry, analytics, or crash reporting is
  implemented.
- **Local operation**: All processing is on your machine. Configuration and logs
  stay local unless you choose to share them (e.g. when opening a bug report).

## Data that may be displayed or stored locally

| Data | Where | Purpose |
|------|--------|---------|
| **Device serial number** | `evtest` only | Shown when you run evtest against a connected device (for local identification). **Not** included in **x52bugreport** output. |
| **Device type info** | `x52bugreport`, `evtest` | Vendor ID, product ID, device version, manufacturer and product name (e.g. "Saitek" / "X52 Pro"). No serial number or hostname in bugreport. |
| **System information** | `x52bugreport` only | Kernel name/release, machine architecture, kernel version string. **Hostname is not included.** |
| **Build environment** | `x52bugreport` only | Compiler, build date, kernel/arch/OS version at build time. No hostname or other machine identifier. |
| **Paths** | Daemon logs (if enabled) | Log file path, config path, socket path. Default paths use system directories (e.g. `/var`, `/run`, `/etc`), not your home directory. |
| **Configuration** | Config files (e.g. under `/etc/x52d/`) | MFD/LED and daemon settings. Stored only on your system. |

None of this data is sent anywhere by the software. The only way it leaves your
system is if you voluntarily paste it (e.g. into a GitHub issue).

## Bug reports

**x52bugreport** output is designed to avoid personal and device identifiers:

- **Device serial number** and **system hostname** are **not** included in the output.
- Included: package/build version, compiler, build date, kernel and machine type, library versions, device vendor/product ID and device name (manufacturer/product strings only).

You may still redact any line before posting if you prefer. For most bugs, the information above is sufficient.

## Your rights (GDPR / CCPA style)

- **No account or sign-up** is required to use the software, so we do not hold
  an account-based profile on you.
- **No selling of data**: We do not collect or sell personal data.
- **Transparency**: This document describes what the software can display or
  store locally.
- **Control**: You decide whether to run `x52bugreport` and what to include when
  opening an issue.

## Third-party services

- **Source and issues**: If you clone the repo or open an issue on GitHub,
  GitHub’s privacy policy applies to that interaction ([GitHub Privacy
  Statement](https://docs.github.com/en/site-policy/privacy-policies/github-privacy-statement)).
- **Packages**: Installation via Ubuntu PPA or Arch AUR is subject to
  Canonical’s or the AUR’s respective terms and privacy practices.

## Changes

We may update this document to reflect changes in the software or in legal
requirements. The current version is in the project repository.
