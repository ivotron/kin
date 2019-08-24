# kin

git-like interface for managing your data.

## commands

Same as Git:

```bash
kin init

kin clone

kin checkout

kin add

kin status

kin commit

kin push
```

## backends

  * zenodo
  * dataverse
  * ckan
  * girder
  * sql
  * s3

maybe:

  * posix
  * dropbox
  * gdrive
  * curl ("single-version, read-only" repos)

## kin-git interaction

Same as `git submodule`:

```bash
kin add path url <backend-options>

kin rm path
```

and the above automatically modifies `.kin.json` and `.gitignore` 
files. The `.kin.json` file:

```json
{
  "uri": "https://zenodo.org/record/439946",
  "type": "zenodo",
  "version": "1"
}
```
