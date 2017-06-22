Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Location: .travis.yml:30
Comment: Nitpick: The comment does not really make sense, no? The next line
         makes Ubuntu trusty available, but 'sudo: required' just provides sudo?

[stg: updated the comment to indicate that 'sudo: required' does not make Ubuntu Trusty available.
Rather, it requires a full VM to be available for testing instead of just a light-weight container.]
