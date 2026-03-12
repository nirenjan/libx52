#!/usr/bin/env python3
"""Generate a changelog for the latest release and dump it to stdout"""

from pathlib import Path

def get_git_root():
    script_path = Path(__file__).resolve()

    # This is always going to reside at <git-root>/.github/scripts/*.py
    scripts_dir = Path(script_path.parent)
    gh_dir = Path(scripts_dir.parent)

    return Path(gh_dir.parent)

def main():
    git_root = get_git_root()

    changelog_file = git_root / 'ChangeLog.md'

    latest = False
    with open(changelog_file) as cfd:
        for line in cfd:
            if line.startswith('## '):
                if 'Unreleased' in line:
                    continue

                if latest:
                    break

                latest = True
                continue

            if latest:
                print(line, end='')


if __name__ == '__main__':
    main()
