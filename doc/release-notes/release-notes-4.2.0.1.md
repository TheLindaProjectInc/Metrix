# Metrix Core v4.2.0.1

## Bugfix Update

This update addresses an issue where the vm.log file can grow to huge sizes if left unattended. The daemon will now trim this file on startup to prevent this excessive bloat.

## Bug Reporting

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

## What's Changed
- Added shrink for the vm.log on daemon startup.