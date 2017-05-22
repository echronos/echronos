Reviewer: David Martin (davidm@brkawy.com)
Conclusion: Rework

Location: prj/app/lib/util/test/util.py:
          prj/app/lib/util/test/crc16.py:
Comment: Instead of using expressions like 'assert x == y' or 'assert x is y'
         it would be good to use the assert methods of the unittest package.
         We are inheriting from unittest.TestCase, so they are available.
         For these cases it would be 'self.assertEqual(x, y)' and
         'self.assertIs(x, y)'.
         The advantage of these is that they print meaningful error messages
         when the assertions fail, such as for example the actual observed
         values that caused the assertion to fail.

[stg: That is a good point.
I have addressed this issue as suggested.]

Location: x_test.py:115
Comment: I did not know about the skipUnless decorator before. I like it!
