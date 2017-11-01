Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Location: x.py:13
Comment:
There is no docstring indicator `"""` at the start of the file so running `x.py` fails.
[clewis: resolved]

Location: all release builds
Comment:
Files in release builds (constructed with `./x.py build release`) have:

    See the file "LICENSE_CSIRO_BSD_MIT.txt" for details.

In their license header, however this file is not present in release builds.
Suggest changing the release build license filename or adding it to release builds.
[clewis: resolved]

Location: many files i.e share/packages/generic/generic-manual.md (in release build `eChronos-std`)
Comment:
In release builds, some files have license headers repeated twice.
[clewis: resolved]

Location: LICENSE.md:12
Comment:
Unnecessary `-->`
[clewis: resolved]

Location: README.md:46
          provenance/pystache/LICENSE:35
          provenance/ply/LICENSE:43
          LICENSE.md:13
          pylib/release.py:181-182
Comment:
I think it is worth switching from 'NICTA' to 'CSIRO Data61' in these places while we're at it.
[clewis: resolved all but LICENSE.md, which is being left as 'NICTA']