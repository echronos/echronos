Reviewer: Sebastian Holzapfel (seb.holzapfel@data61.csiro.au)
Conclusion: Rework

Location: components/task/docs.md:148
          components/gatria/docs.md:96
Comment 1: Rework
These 2 statements seem to conflict.

Location: components/simple-mutex/docs.md:92, 154
Comment 2: Accepted
I don't really like the re-use of 'block' in these places as the 'blocked' task is not actually blocked (from a scheduler perspective), it just spins on a `yield`.
This may be confusing as this RTOS variant actually does have a 'real' blocking API.
I guess from a user's perspective it shouldn't matter, but since this is the main thing distinguishing `simple-mutex` from `blocking-mutex` it may be worth making the distinction.

Location: x.py:137
Comment 3: Accepted
I realise that I also contributed to this, but managing these documentation interdependencies with long dictionaries seems to be getting ugly.
I wonder how difficult it would be to allow the documentation templating engine to query instantiated component names in the variant before the template is generated?

Location: components/gatria/docs.md:63 and elsewhere
Comment 4: Accepted
There are some improvements in clarity that you bring to this documentation that is not present in other variant's documentation, despite the topic being exactly the same.
I'd say it's worth adding these to the other docs while you're at it.
Probably out of scope, but wanted to mention anyway...
